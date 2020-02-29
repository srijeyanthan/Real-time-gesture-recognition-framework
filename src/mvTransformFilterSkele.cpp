/****************************************************************************
*
* mvTransformFilterSkele.cpp
*
* Purpose:  Implementation of mvTransformFilterSkele.h and mvSkeleton.h
*			Forth filter to the pipeline.
*			Calculate extreme points of detedted object.
*			Draw skeletons of that object. 
*			Enerating sequence number according to each individual objects.
* 		    Find the Posture. 
* 		    Find the Gesture.
* 		    Predict the combination of gesture.
*
*
* Author: Sri
*
*
* Class: TransformFilterSkele
* Modules:	TransformFilterSkele::setFont
* 								::setCodebook
* 								::setDhmm
* 								::drawSkele
* 								::sequenceGenerator
* 								::findMin
* 								::findMinPos
* 								::findSkle
* 								::setRenderAction
*								::mvSetImageROItoTracks
*
* Last update date: 23-06-2010
*
******************************************************************************/


#include "mvTransformFilterSkele.h"

using namespace moveit;
using namespace std;


TransformFilterSkele::TransformFilterSkele() : 
tbb::filter(serial_in_order) 
{
	a[0]=0.06;
	a[1]=0.05;
	a[2]=0.045;
	a[3]=0.035;
	a[4]=0.03;

	font=new CvFont;
	setFont();
}  


TransformFilterSkele::~TransformFilterSkele(void)
{
}


// This overloading function is used for run TransformFilterSkele in pipeline.
void* TransformFilterSkele::operator()( void* item ) 
{
	Buffer& b = *static_cast<Buffer*>(item);
	BufferObject *buffer;
	int x=-1;

	for( buffer=b.begin(); buffer!=b.end(); ++buffer ) 
	{
		x=-1;
		IplImage *bgsImage=buffer->getBGSImage();
		IplImage *object_color=buffer->getOriginalImage();
		MvTracks *tracks=buffer->getTracks();
		x=findSkle(tracks,bgsImage,buffer->getFrameNumber(),object_color);

		if(x!=-1)
		{
			throw x;		// Catch errors
		}
		setRenderAction(tracks,object_color);
		
	}

	return &b;  
}


// Print unique object ID to the every object in the frame.
void TransformFilterSkele::setRenderAction(MvTracks *tracks,IplImage *originalImg)
{

	for (MvTracks::const_iterator it=tracks->begin(); it!=tracks->end(); ++it)
	{
	
		if (!it->second->inactive)
		{
			stringstream buffer;
			buffer << it->second->id;
			cvPutText(originalImg, buffer.str().c_str(), cvPoint((int)it->second->centroid.x,(int)it->second->miny-5), font, CV_RGB(255.,0.,0.));
		}
		
	}
}


//Set the font style. 
void TransformFilterSkele::setFont()
{

	cvInitFont(font, CV_FONT_HERSHEY_DUPLEX, .5, .5, 0, 1, CV_AA);
}


//Set the codebook. 
void TransformFilterSkele::setCodebook(ifstream *codebook)
{
	mvFileCodebook=codebook;
}


//Set DHMM. 
void TransformFilterSkele::setDhmm(DHMM * dhm)
{
	dhmm=dhm;
}


// Finds the human extreme poits and store in to vector.
vector<CvPoint2D32f> TransformFilterSkele::drawSkele(IplImage *grey, int x_, int y_, vector<CvPoint2D32f> vec,MvTrack *track,int frameNumber,TrackObj *trackObj){

	vector<int> testx;
	vector<int> testy;
	vector<int> max;
	vector<int> distance;
	vector<double> real;
	vector<double> imaginary;
	vector<double> dft;
	vector<double> idft_real;
	vector<double> idft_imaginary;
	vector<double> idft;
	double localMaximumArray[10];
	int localMaxiumumArrayi[10];
	int count=0;

	IplImage* img_edge = cvCreateImage( cvGetSize(grey), 8, 1 );
 	cvThreshold( grey, img_edge, 128, 255, CV_THRESH_BINARY );

	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* first_contour = NULL;

	int Nc = cvFindContours(
		img_edge,
		storage,
		&first_contour,
		sizeof(CvContour),
		CV_RETR_LIST);  

	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next )
	{
		if(c->h_next==NULL){
			for( int i=0; i < c->total; ++i )
			{
				CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, c, i );
				testx.push_back(p->x);
				testy.push_back(p->y);
			}
		}
	}

	// It performs the DFT , low pass filter and finally IDFT
	double dis =0.0;
	for(int t=0;t<testx.size();++t)
	{
		dis =sqrt((double)((testx[t]-x_)*(testx[t]-x_))+(double)((testy[t]-y_)*(testy[t]-y_)));
		distance.push_back(dis);
	}

	int total_size =distance.size();
	int k1;
	int m1;
	for( k1=0;k1<total_size/2;++k1)
	{
		double real_final=0.0;
		double imag_final=0.0;
		for( m1=0;m1<total_size-1;++m1)
		{
			real_final=distance[m1]*cos(PI*k1*m1/total_size)+real_final;
			imag_final=distance[m1]*sin(PI*k1*m1/total_size)+imag_final;
		}
		real.push_back(real_final);
		imaginary.push_back(imag_final);
	}

	double final_dft=0.0;
	for(int r=0;r<real.size();++r)
	{
		final_dft = sqrt( real[r]*real[r] + imaginary[r]*imaginary[r]);
		dft.push_back(final_dft);
	}

	double psmax=0.0;
	for(int i = 0; i < dft.size(); ++i) 
	{
		if(dft[i] > psmax)
			psmax = dft[i];
	}

	for(int i = 0; i < dft.size(); ++i) 
	{
		dft[i] = dft[i]/psmax;
	}

	for(int g=0;g<4;++g)
	{
		for( k1=0;k1<total_size-1;++k1)
		{
			double real_idft_final=0.0;
			double imag_dft_final=0.0;
			for( m1=0;m1<(a[g]*distance.size() );++m1)
			{
				real_idft_final=(real[m1]*cos(PI*k1*m1/total_size)+imaginary[m1]*sin(PI*k1*m1/total_size))+real_idft_final;
				imag_dft_final=(real[m1]*sin(PI*k1*m1/total_size)-imaginary[m1]*cos(PI*k1*m1/total_size))+imag_dft_final;
			}
			idft_real.push_back(real_idft_final/total_size);
			idft_imaginary.push_back(imag_dft_final/total_size);
		}

		double final_idft=0.0;
		for(int r=0;r<idft_real.size();++r)
		{
			final_idft = sqrt( idft_real[r]*idft_real[r] + idft_imaginary[r]*idft_imaginary[r]);
			idft.push_back(final_idft);
		}
		track->action=trackObj->action;

		int maximum_x_value=idft.size(); 
		for(int j=3; j<maximum_x_value-3;j++)
		{
			for(int k=j-3; k<j+4; k++)
			{
				if(k>j)
				{
					if(idft[j]<idft[k]) break;
				} else if(k<j)
				{
					if(idft[j]<idft[k])break;
				}
				if(k==j+3)
				{
					localMaximumArray[count]= idft[j];
					localMaxiumumArrayi[count]=j;
					count+=1;				
				}
				if(count>6){
					break;
				}
			}
		}

		if(count>6)
		{
			count =0;
			for(int i=0;i<10;++i)
			{
				localMaximumArray[i]= 0;
				localMaxiumumArrayi[i]=0;
			}
			continue;
		}
		else
			break;
	}

	if((count==3) && (abs(total_size-localMaxiumumArrayi[2]-localMaxiumumArrayi[0])<localMaxiumumArrayi[0] || abs(total_size-localMaxiumumArrayi[2]-localMaxiumumArrayi[0])<total_size-localMaxiumumArrayi[2]))	
	{	
		CvPoint2D32f p=cvPoint2D32f(testx[0],testy[0]);
		vec.push_back(p);
		CvPoint2D32f p2=cvPoint2D32f(testx[localMaxiumumArrayi[1]], testy[localMaxiumumArrayi[1]]);
		vec.push_back(p2);
		sequenceGenerator(vec,  x_,  y_,track,frameNumber,trackObj);  
	}else if(count==4)
	{
		if(abs(total_size-localMaxiumumArrayi[3]-localMaxiumumArrayi[0])<localMaxiumumArrayi[0] || abs(total_size-localMaxiumumArrayi[3]-localMaxiumumArrayi[0])<total_size-localMaxiumumArrayi[3])	
		{	
			CvPoint2D32f p=cvPoint2D32f(testx[0],testy[0]);
			vec.push_back(p);
			CvPoint2D32f p2=cvPoint2D32f(testx[localMaxiumumArrayi[1]], testy[localMaxiumumArrayi[1]]);
			vec.push_back(p2);
			CvPoint2D32f p3=cvPoint2D32f(testx[localMaxiumumArrayi[2]], testy[localMaxiumumArrayi[2]]);
			vec.push_back(p3);	
			sequenceGenerator(vec, x_,  y_,track,frameNumber,trackObj);        
		}
	} else 	if(count==5)
	{
		if(abs(total_size-localMaxiumumArrayi[4]-localMaxiumumArrayi[0])<localMaxiumumArrayi[0] || abs(total_size-localMaxiumumArrayi[4]-localMaxiumumArrayi[0])<total_size-localMaxiumumArrayi[4])	
		{	
			CvPoint2D32f p=cvPoint2D32f(testx[0],testy[0]);//testx[0], testy[0]
			vec.push_back(p);
			CvPoint2D32f p2=cvPoint2D32f(testx[localMaxiumumArrayi[1]], testy[localMaxiumumArrayi[1]]);
			vec.push_back(p2);
			CvPoint2D32f p3=cvPoint2D32f(testx[localMaxiumumArrayi[2]], testy[localMaxiumumArrayi[2]]);
			vec.push_back(p3);
			CvPoint2D32f p4=cvPoint2D32f(testx[localMaxiumumArrayi[3]], testy[localMaxiumumArrayi[3]]);
			vec.push_back(p4);
			sequenceGenerator(vec,  x_, y_,track,frameNumber,trackObj);        
		}
	}else if(count==6)
	{
		if(abs(total_size-localMaxiumumArrayi[5]-localMaxiumumArrayi[0])<localMaxiumumArrayi[0] || abs(total_size-localMaxiumumArrayi[5]-localMaxiumumArrayi[0])<total_size-localMaxiumumArrayi[5])	
		{	
			CvPoint2D32f p=cvPoint2D32f(testx[0],testy[0]);
			vec.push_back(p);
			CvPoint2D32f p2=cvPoint2D32f(testx[localMaxiumumArrayi[1]], testy[localMaxiumumArrayi[1]]);
			vec.push_back(p2);
			CvPoint2D32f p3=cvPoint2D32f(testx[localMaxiumumArrayi[2]], testy[localMaxiumumArrayi[2]]);
			vec.push_back(p3);
			CvPoint2D32f p4=cvPoint2D32f(testx[localMaxiumumArrayi[3]], testy[localMaxiumumArrayi[3]]);
			vec.push_back(p4);
			CvPoint2D32f p5=cvPoint2D32f(testx[localMaxiumumArrayi[4]], testy[localMaxiumumArrayi[4]]);
			vec.push_back(p5);
			sequenceGenerator(vec,  x_, y_,track,frameNumber,trackObj);        
		}
	}

	testx.clear();
	testy.clear();
	max.clear();
	distance.clear();
	real.clear();
	imaginary.clear();
	dft.clear();
	idft_real.clear();
	idft_imaginary.clear();
	idft.clear();
	cvReleaseImage(&img_edge );
	cvClearMemStorage(storage );
	if(first_contour!=NULL)
		cvClearSeq(first_contour );
	return vec;

}


// Generating the posture sequence according to incoming postures.
void TransformFilterSkele::sequenceGenerator(vector <CvPoint2D32f > points,int bar_x, int bar_y,MvTrack *track,int frameNumber,TrackObj *trackObj)
{
	vector <string> test;
	string line ;
	vector <double> min;
	vector <double> tempmin;
	vector <int> posmin;
	vector <CvPoint2D32f > temp;
	int x_bar;
	int y_bar;
	double d=0.0;
	int maincount=0;
	vector <int> subcount;
	int seq=0;
	unsigned int trackId=track->id;

	if (mvFileCodebook->is_open())
	{
		while (! mvFileCodebook->eof() )
		{
			getline ((*mvFileCodebook),line);
			test.push_back(line);
			if(line=="0")
			{ 
				maincount =maincount+1;

				if(test.size()== (2*points.size()+3))
				{
					subcount.push_back(maincount);
					for(int i=0;i<=points.size();++i)
					{
						if(i==points.size())
						{
							x_bar= atoi(test[2*i].c_str());
							y_bar= atoi(test[2*i+1].c_str());
						}
						else
						{
							CvPoint2D32f p=cvPoint2D32f(atoi(test[2*i].c_str()), atoi(test[2*i+1].c_str()));
							temp.push_back(p);
						}
					}
					double mintotal=0.0;
					tempmin.clear();

					for(int i=0;i<points.size();++i)
					{
						for(int j=0;j<points.size();++j)
						{
							d= sqrt((temp[i].x-(points[j].x+x_bar-bar_x))*(temp[i].x-(points[j].x+x_bar-bar_x)) +(temp[i].y-(points[j].y+y_bar-bar_y))*(temp[i].y-(points[j].y+y_bar-bar_y)));
							tempmin.push_back(d);
						}
						posmin.push_back(findMinPos(tempmin));

						if(i==0)
						{
							mintotal= mintotal+findMin(tempmin);
						} else if(i==1 && posmin[1]==posmin[0])
						{

							tempmin[posmin[1]]=1000;
							mintotal= mintotal+findMin(tempmin);
						} else if(i==2 && (posmin[2]==posmin[0] ||posmin[2]==posmin[1]) )
						{

							tempmin[posmin[1]]=1000;
							tempmin[posmin[0]]=1000;

							mintotal= mintotal+findMin(tempmin);
						} else if(i==3 && (posmin[3]==posmin[2] ||posmin[3]==posmin[1]||posmin[3]==posmin[0]) )
						{

							tempmin[posmin[2]]=1000;
							tempmin[posmin[1]]=1000;
							tempmin[posmin[0]]=1000;

							mintotal= mintotal+findMin(tempmin);
						} else if(i==4 && (posmin[4]==posmin[3] ||posmin[4]==posmin[2]||posmin[4]==posmin[1]||posmin[4]==posmin[0]) )
						{
							tempmin[posmin[3]]=1000;
							tempmin[posmin[2]]=1000;
							tempmin[posmin[1]]=1000;
							tempmin[posmin[0]]=1000;
							mintotal= mintotal+findMin(tempmin);
						}
					}
					min.push_back(mintotal);	
					temp.clear();
					tempmin.clear();
					posmin.clear();
				}
				test.clear();
			}
		}
		if(subcount.size() != 0)
		{
			seq = subcount[findMinPos(min)];
			trackObj->actionPoints.push_back(seq-1);

		}

		if(trackObj->actionPoints.size()==10)
		{

			list[track->id] = (list.find(track->id)->second + (track->maxy-track->miny))/2 ;
			trackObj->lastX=track->centroid.x;
			if(trackObj->height ==0){
				
				trackObj->height=list.find(track->id)->second;
			}

			track->action="";
			int obsSize = 10;
			Observation obs;
			obs.resize(obsSize,0);
			for (int i = 0; i<obsSize; i++)
			{
				obs[i]=trackObj->actionPoints[i];
			}

			Observation obsx = dhmm->GetProbableStateSequence(obs);
			int noOfGest[7] = {0,0,0,0,0,0,0};
			for (int  j=0; j< obsSize; j++)
			{
				switch (obsx[j])
				{
				case 0 : noOfGest[0]++;break;
				case 1 : noOfGest[1]++;break;
				case 2 : noOfGest[2]++;break;
				case 3 : noOfGest[3]++;break;
				case 4 : noOfGest[4]++;break;
				case 5 : noOfGest[5]++;break;
				case 6 : noOfGest[6]++;break;

				}
			}

			int mostProbable = 0;
			int maxGest = 0;
			for (int k =0; k<7; k++)
			{
				if (noOfGest[k] > maxGest)
				{
					maxGest =noOfGest[k];
					mostProbable = k;
				}
			}
			int u=list.find(track->id)->second/10;
			int h=( (int) (  list.find(track->id)->second - trackObj->height) );
			if(mostProbable==0)
			{
				if(abs((int)(trackObj->lastX - trackObj->beginX))>45){
					track->action="running";
				} else {
					track->action="walking";
				}
				trackObj->height=(trackObj->height + list.find(track->id)->second) /2;
			}else if(mostProbable==1)
			{
				if((trackObj->action).compare("asking help!")==0)
					track->action="asking help!";
				else 
					track->action="standing";
				trackObj->height=(trackObj->height + list.find(track->id)->second) /2;
			}else if(mostProbable==2)
			{
				track->action="sitting";
				if(trackObj->height==0)
					trackObj->height=list.find(track->id)->second;
			} else 	if(mostProbable==3){
				track->action="falling";
				if(trackObj->height==0)
					trackObj->height=list.find(track->id)->second;
			} else if(mostProbable==4)
			{
				track->action="bending";
				if(trackObj->height==0)
					trackObj->height=list.find(track->id)->second;
			}else if(mostProbable==5)
			{
				track->action="crawling";
				if(trackObj->height==0)
					trackObj->height=list.find(track->id)->second;
			} else if(mostProbable==6){
				if(trackObj->height == 0){
					track->action="falling";
					if(trackObj->height==0)
					trackObj->height=list.find(track->id)->second;
				}
				else if(   h >20) {
					track->action="asking help!";
				}
				else{
					
					if((trackObj->action).compare("asking help!")==0)
						track->action="asking help!";
					else{
						track->action=trackObj->action;
						trackObj->height=(trackObj->height + list.find(track->id)->second) /2;
					}

				}
			}





			if  (trackObj->behaviour.size() == 0)
			{
				trackObj->uncommon.push_back(1);
				trackObj->uncommon.push_back(6);
				trackObj->uncommonpopvalue = 100;
				for (int i = 0; i< trackObj->uncommon.size(); i++)
				{
					uncommonSample.push_back(trackObj->uncommon.at(i));
				}
			}

			trackObj->behaviour.push_back(mostProbable);

			if(mostProbable==4){
				trackObj->abnorm=true;
			}

			if (trackObj->uncommon.back() == mostProbable)
			{
				trackObj->uncommonpopvalue = trackObj->uncommon.back();
				trackObj->uncommon.pop_back();
				if(trackObj->uncommon.size()== 0)
				{
					trackObj->abnorm=true;

					for (int j = 0; j< uncommonSample.size(); j++)
					{
						trackObj->uncommon.push_back(uncommonSample.at(j));
					}

				}
			}
			else if (trackObj->uncommonpopvalue != mostProbable)
			{
				trackObj->uncommon.clear();
				for (int i = 0; i< uncommonSample.size(); i++)
				{
					trackObj->uncommon.push_back(uncommonSample.at(i));
				}
			}


			if(track->detectId>0 && trackObj->abnorm){
					track->action = "Bag drop!";
			}

			list[track->id] =0;
			trackObj->action=track->action;
			trackObj->actionPoints.clear();
		}else{
			if(trackObj->actionPoints.size()==1)
			{
				trackObj->beginX=track->centroid.x;
			} 
			if(list.find(track->id)->second !=0)
				list[track->id] = (list.find(track->id)->second + (track->maxy-track->miny))/2 ;
			else
				list[track->id] = (track->maxy-track->miny) ;



			//int x=frameNumber;
			track->action=trackObj->action;
		}
		min.clear();
		mvFileCodebook->clear();
		mvFileCodebook->seekg(0,ios_base::beg);
	} else 
		throw 5;
	subcount.clear();
	test.clear();
	min.clear();
	tempmin.clear();
	posmin.clear();
	temp.clear();
}


// Find the minimum posture sequence number from minx array.
double TransformFilterSkele::findMin( std::vector<double> minx)
{
	double minimum = minx[0];
	for(int j=0;j<minx.size();++j)
	{
		if(minx[j]<minimum)
			minimum = minx[j];
	}

	return minimum;
}


// Find where the minimum value occured. 
int  TransformFilterSkele::findMinPos(std::vector<double> minp)
{
	double minimum = minp[0];
	int pos =0;
	for(int j=0;j<minp.size();++j)
	{
		if(minp[j]<minimum)
		{
			minimum = minp[j];
			pos =j;
		}	 
	}
	return pos;
}


//Set the Image Region of Interest for tracks.
void TransformFilterSkele::mvSetImageROItoTracks(IplImage *img, MvTrack const *track)
{
	cvSetImageROI(img, cvRect(track->minx, track->miny, track->maxx-track->minx, track->maxy-track->miny));
};



// Performs all the skelton function such as draw, add text. 
int TransformFilterSkele::findSkle(MvTracks *tracks,IplImage *bgs,int frameNumber,IplImage *tt)
{
	vector <CvPoint2D32f > currentpoints;
	unsigned int trackId=0;
	for (MvTracks::const_iterator jt = tracks->begin(); jt!=tracks->end();jt++){

		trackId=jt->second->id;

		if(cpyTrack.count(trackId)==0){
			TrackObj *trackObj=new TrackObj;
			trackObj->height=0;
			trackObj->abnorm=false;
			cpyTrack.insert(cpyLabelTrack(trackId,trackObj));
			list[jt->second->id] =0;
		}
		cpyTracks::iterator cpyTrackIterator= cpyTrack.find(trackId);

		if(jt->second->merge!=true){
			mvSetImageROItoTracks(bgs,jt->second);

			try{
				currentpoints = drawSkele(bgs, (jt->second->centroid.x - jt->second->minx), (jt->second->centroid.y - jt->second->miny),currentpoints,jt->second,frameNumber,cpyTrackIterator->second);
			}catch(...){
				return 5;
			}

			currentpoints.clear();
		}
	}

	return -1;
}
