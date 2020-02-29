/****************************************************************************
*
* mvTransformFilterTracks.cpp
*
* Purpose:  Implementation of mvTransformFilterTracks.h
*			Third filter to the pipeline.
*			Concreate class of mvTrack.
*
* Author: Nihanth
*
*
* Class: TransformFilterTracks
* Modules:	TransformFilterTracks::findPairs
*								 ::tracking
*								 ::renderTracks
*								 ::naiveNearestNeighbor
*								 ::compareSURFDescriptors
*								 ::mvCopyTrack
*								 ::doBoundingBoxesIntersect
*							     ::mvSetImageROItoBlob
*
* Last update date: 23-06-2010
*
******************************************************************************/


#include "mvTransformFilterTracks.h"
#include <sstream>

using namespace std;


TransformFilterTracks::TransformFilterTracks(IplImage *image) : tbb::filter(serial_in_order) 
{
	font =  new CvFont;
	cvInitFont(font, CV_FONT_HERSHEY_DUPLEX, 0.5, 0.5, 0, 1);		// Initialize font
	maxTrackID=0;													//maxTrackID is a unique obj tracking ID
	grey = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
	prevGrey = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );
	thInactive=50;													//thInactive indicate how many frames a object can be inactivate position
	tracks=new MvTracks;
}  


TransformFilterTracks::~TransformFilterTracks(void)
{}


// This function is used for run TransformFilterTracks in pipeline. 
void* TransformFilterTracks::operator()( void* item ) 
{
	Buffer& b = *static_cast<Buffer*>(item);
	IplImage *swapTemp;
	BufferObject *buffer;

	for( buffer=b.begin(); buffer!=b.end(); ++buffer ) 
	{
		IplImage *image=buffer->getOriginalImage();
		blobs=buffer->getBlobs();
		cvCvtColor( image, grey, CV_BGR2GRAY );
		maxTrackID=tracking(*blobs, *tracks, prevGrey, grey, thInactive,maxTrackID);
		MvTracks *cpyTrack = mvCopyTrack(buffer->getFrameNumber(),tracks);	
		buffer->setTracks(cpyTrack);
		CV_SWAP( prevGrey, grey, swapTemp );
		releaseBlobs(*blobs);
	}

	return &b;  
}


//Make a dupicate copy of Track.	 
MvTracks* TransformFilterTracks::mvCopyTrack(int x, MvTracks *tracks)
{
	MvTracks *t=new MvTracks;
	for (MvTracks::iterator it=tracks->begin(); it!=tracks->end(); it++)
	{
		MvTrack *track = it->second;
		MvTrack *temp = new MvTrack;
		temp->minx=track->minx;
		temp->label=track->label;
		temp->miny=track->miny;
		temp->maxx=track->maxx;
		temp->maxy=track->maxy;
		temp->id=track->id;
		temp->abnorm=false;
		temp->merge=track->merge;
		temp->centroid=track->centroid;
		temp->inactive=track->inactive;
		temp->detectId=track->detectId;
		temp->color=track->color;
		t->insert(MvIDTrack(it->second->id,temp));
	}

	return t;
}


// Find merged area of two boudning boxes of bolbs. This was done for previous and current frame's  blobs.
int TransformFilterTracks::doBoundingBoxesIntersect(int ATLx, int ATLy, int ABRx, int ABRy, int BTLx, int BTLy, int BBRx, int BBRy) 
{
	int rabx = abs(ATLx + ABRx - BTLx - BBRx);
	int raby = abs(ATLy + ABRy - BTLy - BBRy);
	int raxPrbx = abs(ABRx - ATLx + BBRx - BTLx);
	int rayPrby = abs(ABRy - ATLy + BBRy - BTLy);

	if(rabx <= raxPrbx && raby <= rayPrby)
	{
		int x_overlap = MIN( BBRx, ABRx )-MAX( BTLx, ATLx );
		int y_overlap = MIN( BBRy, ABRy )-MAX( BTLy, ATLy );
		return ( abs((x_overlap)*(y_overlap)));
	}else 
		return 0;
}


// Compare the SURF Descriptors.
double TransformFilterTracks::compareSURFDescriptors( const float* d1, const float* d2, double best, int length )
{
	double total_cost = 0;
	assert( length % 4 == 0 );
	for( int i = 0; i < length; i += 4 )
	{
		double t0 = d1[i] - d2[i];
		double t1 = d1[i+1] - d2[i+1];
		double t2 = d1[i+2] - d2[i+2];
		double t3 = d1[i+3] - d2[i+3];
		total_cost += t0*t0 + t1*t1 + t2*t2 + t3*t3;
		if( total_cost > best )
			break;
	}
	return total_cost;
}


// Finds the two nearest neighbors, but only returns the very nearest if it is distinctly better than the second nearest.
int TransformFilterTracks::naiveNearestNeighbor( const float* vec, int laplacian, const CvSeq* model_keypoints, const CvSeq* model_descriptors )
{
	int length = (int)(model_descriptors->elem_size/sizeof(float));
	int i, neighbor = -1;
	double d, dist1 = 1e6, dist2 = 1e6;
	CvSeqReader reader, kreader;
	cvStartReadSeq( model_keypoints, &kreader, 0 );
	cvStartReadSeq( model_descriptors, &reader, 0 );

	for( i = 0; i < model_descriptors->total; i++ )
	{
		const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
		const float* mvec = (const float*)reader.ptr;
		CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
		CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
		if( laplacian != kp->laplacian )
			continue;
		d = compareSURFDescriptors( vec, mvec, dist2, length );
		if( d < dist1 )
		{
			dist2 = dist1;
			dist1 = d;
			neighbor = i;
		}
		else if ( d < dist2 )
			dist2 = d;
	}
	if ( dist1 < 0.6*dist2 )
		return neighbor;

	return -1;
}


// Search same features in two objects and return matched fatures.
void TransformFilterTracks::findPairs( const CvSeq* objectKeypoints, const CvSeq* objectDescriptors,const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, vector<int>& ptpairs )
{
	CvSeqReader reader, kreader;
	cvStartReadSeq( objectKeypoints, &kreader );
	cvStartReadSeq( objectDescriptors, &reader );
	ptpairs.clear();

	for(int i = 0; i < objectDescriptors->total; i++ )
	{
		const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
		const float* descriptor = (const float*)reader.ptr;
		CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
		CV_NEXT_SEQ_ELEM( reader.seq->elem_size, reader );
		int nearest_neighbor = naiveNearestNeighbor( descriptor, kp->laplacian, imageKeypoints, imageDescriptors );

		if( nearest_neighbor >= 0 )
		{
			ptpairs.push_back(i);
			ptpairs.push_back(nearest_neighbor);
		}
	}
}


// Track the object using bounding box overlapping method. 
MvID TransformFilterTracks::tracking(MvBlobs const &blobs, MvTracks &tracks,  IplImage *frame, IplImage *currentframe,int thInactive ,MvID maxTrackID)
{
	MvBlobs preBlob;
	for (MvTracks::const_iterator jt = tracks.begin(); jt!=tracks.end(); ++jt)
	{
		MvBlob *blob = new MvBlob;	
		blob->label = jt->second->id;		
		blob->minx = jt->second->minx;
		blob->miny = jt->second->miny;
		blob->maxx = jt->second->maxx;				
		blob->maxy = jt->second->maxy;
		preBlob.insert(MvLabelBlob(jt->second->id,blob));
	}

	bool trackFound = false;
	//For every available tracks
	for (MvTracks::const_iterator jt = tracks.begin(); jt!=tracks.end(); )
	{
		trackFound = false;
		MvTrack *track = jt->second;
		int blobLabel = 0;
		int splitID = 0;
		int trackArea = 0;
		float ratio = 0;
		trackArea = (track->maxx - track->minx) * (track->maxy - track->miny) ;
		int heightMatch=1000;
		
		if(track->merge == true)	// If track is merged
		{   
			float percentageOfmatch=0;

			for (MvBlobs::const_iterator it = blobs.begin(); it!=blobs.end(); ++it)
			{				
				ratio = (float)doBoundingBoxesIntersect(it->second->minx, it->second->miny, it->second->maxx, it->second->maxy, track->minx, track->miny, track->maxx, track->maxy) / trackArea;

				if( ratio >= 0.7)	// Objects are still merged
				{   
					blobLabel = it->second->label;
					trackFound = true;
					splitID = 0;
					break;
				} else if (ratio >= .15) 
				{ 
					// Objects are splited
					mvSetImageROItoBlob(currentframe,it->second);
					CvMemStorage* storage = cvCreateMemStorage(0);
					CvSeq *objectKeypoints = 0, *objectDescriptors = 0;
					CvSeq *imageKeypoints = 0, *imageDescriptors = 0;
					CvSURFParams params = cvSURFParams(1000, 1);
					cvExtractSURF( currentframe, 0, &imageKeypoints, &imageDescriptors, storage, params );
					vector<int> firstptpairs;
					vector<int> secondptpairs;
					float match = 0;

					if(percentageOfmatch<match)
					{
						percentageOfmatch=match;	
					}

					int temHeight = abs((int)jt->second->height - (int)(it->second->maxy - it->second->miny));
					
					if(temHeight < heightMatch)
					{
						heightMatch = temHeight;
						blobLabel = it->second->label;
						splitID = blobLabel;
						trackFound = true;	
					}

					cvResetImageROI(currentframe);
				}
			}
		} 
		else
		{ 
			//If Track is individual track
			for (MvBlobs::const_iterator it = blobs.begin(); it!=blobs.end(); ++it)
			{
				ratio = (float) doBoundingBoxesIntersect(it->second->minx, it->second->miny, it->second->maxx, it->second->maxy, track->minx, track->miny, track->maxx, track->maxy) / trackArea;

				if(ratio > .4)
				{
					blobLabel = it->second->label;
					trackFound = true;
					splitID = 0;
					break;
				}else 
				{
					splitID=0;
				}
			}
		}

		if(trackFound == true)	// If track is found
		{
			// Update the track positions
			if(splitID>0)
			{
				track->merge=false;
				track->inactive=0;
			}

			if(blobLabel != 0)
			{
				MvBlob *blob = blobs.find(blobLabel)->second;
				track->minx = blob->minx;
				track->miny = blob->miny;
				track->maxx = blob->maxx;				
				track->maxy = blob->maxy;
				track->centroid = blob->centroid;	
				track->label = blob->label;
			}

			jt++;
		}else 
		{
			delete track;
			tracks.erase(jt++);		
		}	
	}

	bool found=false;
	for (MvBlobs::const_iterator it = blobs.begin(); it!=blobs.end(); ++it)
	{
		found=false;
		for (MvTracks::const_iterator jt = tracks.begin(); jt!=tracks.end(); ++jt)
		{
			if(it->second->label == jt->second->label)
			{
				found=true;
				break;
			}
		}

		if(found == false )	// If new object entered into the screen
		{
			//update the track
			maxTrackID++;
			MvTrack *track = new MvTrack;
			MvBlob *blob = it->second;
			track->id = maxTrackID;	
			track->label = blob->label;		
			track->minx = blob->minx;
			track->miny = blob->miny;
			track->maxx = blob->maxx;				
			track->maxy = blob->maxy;
			track->action="";
			track->abnorm=false;
			track->detectId=0;
			int intersectArea=0;
			track->centroid = blob->centroid;

			// If new track entered into the screen from middle 
			if(blob->centroid.x < 250 && blob->centroid.x > 50)
			{
				for (MvBlobs::const_iterator ite = preBlob.begin(); ite!=preBlob.end(); ++ite)
				{
					intersectArea=  doBoundingBoxesIntersect(ite->second->minx, ite->second->miny, ite->second->maxx, ite->second->maxy, blob->minx, blob->miny, blob->maxx, blob->maxy);
					if(intersectArea > 5)
					{
						MvBlobs::iterator  temp = preBlob.find(ite->second->label);
						track->detectId = temp->second->label;
						MvTracks::iterator t = tracks.find(ite->second->label);
						t->second->detectId=maxTrackID;
					}
				}
			}

			track->inactive = 0;
			track->merge = false;
			track->parent = 0;
			track->color = CV_RGB(rand()&255,rand()&255,rand()&255);
			tracks.insert(MvIDTrack(maxTrackID, track));
		} 
	}

	// Check whether two obecjts are merged
	for (MvTracks::const_iterator jt = tracks.begin(); jt!=tracks.end(); ++jt)
	{
		if(jt->second->merge != true)
		{
			for (MvTracks::const_iterator jt1 = tracks.begin(); jt1!=tracks.end(); ++jt1)
			{
				if(jt->second->label == jt1->second->label && jt->second->id != jt1->second->id )
				{
					if(jt->second->merge != true)
					{
						MvBlobs::iterator  temp = preBlob.find(jt->second->id);
						jt->second->merge=true;

						if(jt1->second->merge == true)
							jt->second->parent=jt1->second->parent;
						else 
							jt->second->parent=jt->second->id;

						jt->second->height=temp->second->maxy -temp->second->miny;
						mvSetImageROItoBlob(frame,temp->second);
						CvMemStorage* storage = cvCreateMemStorage(0);
						CvSeq *objectKeypoints = 0, *objectDescriptors = 0;
						CvSeq *imageKeypoints = 0, *imageDescriptors = 0;
						CvSURFParams params = cvSURFParams(1000, 1);
						jt->second->objectDescriptors = objectDescriptors;
						jt->second->objectKeypoints = objectKeypoints;
						cvResetImageROI(frame);
					}

					if(jt1->second->merge !=true)
					{
						MvBlobs::iterator  temp = preBlob.find(jt1->second->id);
						jt1->second->merge=true;

						if(jt->second->merge == true)
							jt1->second->parent=jt->second->parent;
						else 
							jt1->second->parent=jt->second->id;

						jt1->second->height=temp->second->maxy -temp->second->miny;
						mvSetImageROItoBlob(frame,temp->second);
						CvMemStorage* storage = cvCreateMemStorage(0);
						CvSeq *objectKeypoints = 0, *objectDescriptors = 0;
						CvSeq *imageKeypoints = 0, *imageDescriptors = 0;
						CvSURFParams params = cvSURFParams(1000, 1);
						jt1->second->objectDescriptors = objectDescriptors;
						jt1->second->objectKeypoints = objectKeypoints;
						cvResetImageROI(frame);
					}
				}
			}
		}
	}

	
	//Delete inactivate tracks
	for (MvTracks::const_iterator jt = tracks.begin(); jt!=tracks.end();)
	{
		if(jt->second->merge == true)
		{
			if(jt->second->parent != jt->second->id)
			{
				jt->second->inactive++;
			}

			if(jt->second->inactive > thInactive)
			{
				delete jt->second;
				tracks.erase(jt++);
			}else
			{
				jt++;
			}
		} else 
		{
			jt++;
		}
	}

	preBlob.clear();
	return maxTrackID;
}


//  Set the Image Region of Interest for Blobs.	
void TransformFilterTracks::mvSetImageROItoBlob(IplImage *img, MvBlob const *blob)
{
	cvSetImageROI(img, cvRect(blob->minx, blob->miny, blob->maxx-blob->minx, blob->maxy-blob->miny));
}


//Draw bounding box over a blob and put unique blob ID.
void TransformFilterTracks::renderTracks(MvTracks const tracks, IplImage *imgDest, unsigned short mode)
{
	if (mode)
	{
		for (MvTracks::const_iterator it=tracks.begin(); it!=tracks.end(); ++it)
		{
			if (mode&CV_TRACK_RENDER_ID)
			{
				if (!it->second->inactive)
				{
					stringstream buffer;
					buffer << it->first;
					cvPutText(imgDest, buffer.str().c_str(), cvPoint((int)it->second->centroid.x,(int)it->second->miny-5), font, CV_RGB(0.,255.,0.));
				}
			}

			if (mode&CV_TRACK_RENDER_BOUNDING_BOX)
			{
				if (!it->second->inactive)
				{
					cvRectangle(imgDest, cvPoint(it->second->minx, it->second->miny), cvPoint(it->second->maxx, it->second->maxy), it->second->color,3);
				}
			}
		}
	}

}
