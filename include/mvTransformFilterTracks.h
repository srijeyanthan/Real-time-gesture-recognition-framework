/****************************************************************************
*
* mvTransformFilterTracks.h
*
* Purpose: Concreate class of mvTrack
*
* Author: Nihanth
* 
* 
* Class: TransformFilterTracks
* Modules:	mvTrack::findPairs
*				   ::tracking
*				   ::renderTracks
*				   ::naiveNearestNeighbor
*				   ::compareSURFDescriptors
*				   ::mvCopyTrack
*				   ::releaseBlobs
*				   ::doBoundingBoxesIntersect
*				   ::mvSetImageROItoBlob
*
* [1]	Module name:	  findPairs
*		Purpose:		  Search same features in two objects and return matched fatures.
* 		Last update date: 22-06-2010
*
* [2]	Module name:	  tracking
*		Purpose:		  Track the object using bounding box overlapping method. 
*		Reference:		  Gary Bradski and Adrian Kaehler, Learning OpenCV,Gravenstein Highway North, Sebastopol,CA, 2008.
*		Algorithm:		  Bounding Box Overlapping Algorithm:
*
*						  Oi(t): Object i at time=t (current frame)
*						  Oj(t-1): Object j at time = t-1 (previous frame)
*						  Bi(t): Bounding box of object i at time=t (current frame)
*						  Bj(t-1): Bounding box of object j at time=t-1 (previous frame)
*						  Vj(t-1): velocity of the object j at time=t-1 (previous frame)
*
*						  IF box overlapping (Bi(t),Bj(t-1)+Vj(t-1)) > threshold THEN
*							Oi(t) == Oj(t-1)
*						  ELSE
*							Oi(t) Oj(t-1)
*
*
*						- Detect the moving blob (using background subtraction)
*						- Label the image and draw bounding box
*						- Get the previous frame's track
*						- Calculate the Overlapped Area / Tracking BoundingBox Area. 
*						- IF that value is greater than threshold values (0.5) THEN both are same blob ELSE both are different blobs
*						- IF both are same blobS THEN update the track position based on current blob's bounding box details ELSE start new track
*
*						//Merging and splitting 
*						- IF blobs are merged, THEN get features of individual blob's features AND store those feature information to their	`	**						  respective tracks.
*						- Track blobs while they merged.
*						- When objects are splitted, then compare splitted blob'S features (grey scale image) against track features
*						- IF both feature are matched THEN both are same blob.
*
*		Last update date: 22-06-2010
*
* [3]	Module name:	  renderTracks
*		Purpose:		  Draw bounding box over a blob and put unique blob ID. 
*		Last update date: 22-06-2010
*
* [4]	Module name:	  naiveNearestNeighbor
*		Purpose:		  Finds the two nearest neighbors, but only returns the very nearest if it is distinctly better than the second nearest. 
*		Reference:		  Gary Bradski and Adrian Kaehler, Learning OpenCV,Gravenstein Highway North, Sebastopol,CA, 2008.
*		Last update date: 22-06-2010
*
* [5]	Module name:	  compareSURFDescriptors
*		Purpose:		  Compare the SURF Descriptors.
*		Reference:		  Gary Bradski and Adrian Kaehler, Learning OpenCV,Gravenstein Highway North, Sebastopol,CA, 2008.
*		Last update date: 22-06-2010
*
* [6]	Module name:	  mvCopyTrack
*		Purpose:		  Make a dupicate of Track.	 
*		Last update date: 22-06-2010
*
* [7]	Module name:	  releaseBlobs
*		Purpose:		  Clear blobs vector one by one.	 
*		Last update date: 22-06-2010
*
* [8]	Module name:	  doBoundingBoxesIntersect
*		Purpose:		  Find merged area of two boudning boxes of bolbs. This was done for previous and current frame's  blobs.
*		Algorithm:		  Determining if two bounding boxes overlap.
*	
*						//First bounding box, top left corner, bottom right corner
*						ATLx Blob-A Top Left X-axis
*						ATLy Blob-A Top Left Y-axis
*						ABRx Blob-A Bottom Right X-axis
*						ABRy Blob-A Bottom Right Y-axis
*
*						//Second bounding box, top left corner, bottom right corner
*						BTLx Blob-B Top Left X-axis
*						BTLy Blob-B Top Left Y-axis
*						BBRx Blob-B Bottom Right X-axis
*						BBRy Blob-B Bottom Right Y-axis
*
*						rabx = Math.abs(ATLx + ABRx - BTLx - BBRx);
*						raby = Math.abs(ATLy + ABRy - BTLy - BBRy);
*
*						raxPrbx = ABRx - ATLx + BBRx - BTLx;
*						rayPrby = ATLy - ABRy + BTLy - BBRy;
*
*						IF (rabx <= raxPrbx && raby <= rayPrby) THEN
*							blobs are overlapped
*						ELSE
*							not overlapped
*
*		Last update date: 22-06-2010
*
* [9]	Module name:	  mvSetImageROItoBlob
*		Purpose:		  Set the Image Region of Interest for Blobs.	 
*		Last update date: 22-06-2010
*
******************************************************************************/


#pragma once

#include "mvTransformFilter.h"
#include "moveItTrack.h"
#include <fstream>


class TransformFilterTracks: public tbb::filter, public mvTrack
{

private:
	IplImage *grey, *prevGrey, *swapTemp;	// grey: current frame's grayscale image, prevGrey: previous grayscale image, swapTemp: temporary image.
	MvTracks *tracks;						// tracks
	MvBlobs *blobs;							// blobs
	MvID maxTrackID;						// Unique ID to track image
	int thInactive;							// Inactive track
	CvFont *font;							// Font to write text


public:
	TransformFilterTracks(IplImage *image);
	~TransformFilterTracks(void);

	/*overridevoid*/ void* operator()( void* item );	// Operator overloading function is used for run TransformFilterTracks in pipeline. 
	void releaseBlobs(MvBlobs &blobs)
	{
		for (MvBlobs::iterator it=blobs.begin(); it!=blobs.end(); it++)
		{
			MvBlob *blob = (*it).second;
			if (blob) delete blob;
		}

		blobs.clear();
	}

	MvTracks* mvCopyTrack(int, MvTracks *tracks);
	int doBoundingBoxesIntersect(int ATLx, int ATLy, int ABRx, int ABRy, int BTLx, int BTLy, int BBRx, int BBRy);
	double compareSURFDescriptors( const float* d1, const float* d2, double best, int length );
	int naiveNearestNeighbor( const float* vec, int laplacian, const CvSeq* model_keypoints, const CvSeq* model_descriptors );
	void findPairs( const CvSeq* objectKeypoints, const CvSeq* objectDescriptors,const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, vector<int>& ptpairs );
	MvID tracking(MvBlobs const &blobs, MvTracks &tracks,  IplImage *frame, IplImage *currentframe,int thInactive ,MvID maxTrackID);
	void renderTracks(MvTracks const tracks, IplImage *imgDest, unsigned short mode);
	void mvSetImageROItoBlob(IplImage *img, MvBlob const *blob);

};
