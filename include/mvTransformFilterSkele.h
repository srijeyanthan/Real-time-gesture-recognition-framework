/****************************************************************************
*
* mvTransformFilterSkele.h
*
* Purpose: Implementation of mvSkeleton.h. Transform Filter Skele is used to:
* 		   - Draw skeletons of detected object. 
* 		   - Find the Posture. 
* 		   - Find the Gesture.
* 		   - Predict the combination of gesture.
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
* [1]	Module name:	  setFont
*		Purpose:		  Set the font style. 
*		Last update date: 22-06-2010
*
* [2]	Module name:	  setCodebook
*		Purpose:		  Set the codebook. 
*		Last update date: 22-06-2010
*
* [3]	Module name:	  setDhmm
*		Purpose:		  Set DHMM. 
*		Last update date: 22-06-2010
*
* [4]	Module name:	  drawSkele
*		Purpose:		  Finds the human extreme poits and store in to vector.
*		Algorithm:		  Skeletonization:
*						  1) The centroid of the target image boundary (xc; yc) is determined.
*
*						  2) The distance di from the centroid (Xc; Yc) to each border point (xi; yi)
*						     are calculated these are expressed as a one dimensional discrete function
*							 d(i)= d^(i). Note that this function is periodic with period N
*
*						  3) The signal d(i) is then smoothed for noise reduction , becoming d^(i)this 
*							 can be done using a linear smoothing filter or low pass filtering in the
*							 Fourier domain.
*
*						  4) Local maxima of d^(i) are taken as extreme points, and the "star" skeleton
*							 is constructed by connecting then to the target centroid (Xc; Yc). 
*							 Local maxima are detected by finding zero-crossings of the difference function.
*
*						  After identified the Skelton, then that skeleton will be matched with codebook
*						  in order to predict the best matching sequence. This is basically done using vector
*						  length between incoming postures and predefined postures. Then, find the minimum
*						  total and predict the corresponding postures sequence.
*		Last update date: 22-06-2010
*
* [5]	Module name:	  sequenceGenerator
*		Purpose:		  To find the best match posture.
*						  To find rhe best match gesture.
*						  To detect the abnormal behaviour.
*		Algorithm:		  Gesture Analysis:
*						  Each found postures are inserted into one vector.
*						  Then, by calling "dhmm->GetProbableStateSequence" method correspondent gesture of each posture will be found.
*						  Then, by analysing a set of postures and its most suitable gesture, the gesture gets found.
*		Last update date: 22-06-2010
*
* [6]	Module name:	  findMin
*		Purpose:		  Find the minimum posture sequence number from minx array.
*		Last update date: 22-06-2010
*
* [7]	Module name:	  findMinPos
*		Purpose:		  Find where the minimum value occured. 
*		Last update date: 22-06-2010
*
* [8]	Module name:	  findSkle
*		Purpose:		  Performs all the skelton function such as draw, add text. 
*		Last update date: 22-06-2010
*
* [9]	Module name:	  setRenderAction
*		Purpose:		  Print unique object ID to the every object in the frame.
*		Last update date: 22-06-2010
*
* [10]	Module name:	  mvSetImageROItoTracks
*		Purpose:		  Set the Image Region of Interest for tracks.
*		Last update date: 22-06-2010
*
******************************************************************************/


#pragma once

#include "mvTransformFilterTracks.h"
#include "mvSkeleton.h"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

#define PI (3.1415926536)


class TransformFilterSkele: public tbb::filter, public Skeleton
{

private:
	typedef unsigned int height;
	typedef std::map<MvLabel, height> HeightList;			// Store list of height 
	typedef std::pair<MvLabel, height>  HeightListPair;		// Pair of HeightList
	typedef std::map<MvLabel,TrackObj *> cpyTracks;			// map to store duplicate Track
	typedef std::pair<MvLabel, TrackObj *>  cpyLabelTrack;	// Pair of cpyTracks

	double a[5];
	ifstream *mvFileCodebook;
	DHMM * dhmm;
	HeightList list;
	CvFont *font;											// Font to print
	cpyTracks cpyTrack;										// Instant of copy Ttrack
	vector <int> uncommonSample;


public:
	TransformFilterSkele();
	~TransformFilterSkele(void);

	/*overridevoid*/ void* operator()( void* item );		// Operator overloading function is used for run TransformFilterSkele in pipeline.
	void setFont();
	void setCodebook(ifstream *codebook);
	void setDhmm(DHMM * dhm);
	vector<CvPoint2D32f> drawSkele(IplImage *grey, int x_, int y_, vector<CvPoint2D32f> vec,MvTrack *track,int frameNumber,TrackObj *trackObj);
	void sequenceGenerator(vector <CvPoint2D32f > points,int bar_x, int bar_y,MvTrack *track,int frameNumber,TrackObj *trackObj);
	double findMin( std::vector<double> minx);
	int  findMinPos(std::vector<double> minp);
	int findSkle(MvTracks *tracks,IplImage *bgs,int frameNumber,IplImage *);
	void setRenderAction(MvTracks *tracks,IplImage *originalImg);
	void mvSetImageROItoTracks(IplImage *img, MvTrack const *track);


};
