/****************************************************************************
*
* mvSkeleton.h
*
* Purpose: Base Class of skeletonization. This abstract class is used to:
* 		   - Draw skeletons of detected object. 
* 		   - Find the Posture. 
* 		   - Find the Gesture.
* 		   - Predict the combination of gesture.
*
* Author: Sri
*
*
* Class: Skeleton
* Modules:	Skeleton::setFont
* 					::setCodebook
* 					::setDhmm
* 					::drawSkele
* 					::sequenceGenerator
* 					::findMin
* 					::findMinPos
* 					::findSkle
* 					::setRenderAction
*
* [1]	Module name:	  setFont
*		Purpose:		  Set the font style. 
*		Last update date: 09-06-2010
*
* [2]	Module name:	  setCodebook
*		Purpose:		  Set the codebook. 
*		Last update date: 09-06-2010
*
* [3]	Module name:	  setDhmm
*		Purpose:		  Set DHMM. 
*		Last update date: 09-06-2010
*
* [4]	Module name:	  drawSkele
*		Purpose:		  This function performs calculating extreme points of human objects and store in to vector.  
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
*		Last update date: 09-06-2010
*
* [5]	Module name:	  sequenceGenerator
*		Purpose:		  To find the best match posture.
*						  To find rhe best match gesture.
*						  To detect the abnormal behaviour.
*		Algorithm:		  Gesture Analysis:
*						  Each found postures are inserted into one vector.
*						  Then, by calling "dhmm->GetProbableStateSequence" method correspondent gesture of each posture will be found.
*						  Then, by analysing a set of postures and its most suitable gesture, the gesture gets found.
*		Last update date: 09-06-2010
*
* [6]	Module name:	  findMin
*		Purpose:		  Actually, this function using by sequenceGenerator function, when we match output postures with codebook there can be a*						  more than one postures. This function find the minimum matching postures.
*		Last update date: 09-06-2010
*
* [7]	Module name:	  findMinPos
*		Purpose:		  This function finds the where minimum value occurred.
*		Last update date: 09-06-2010
*
* [8]	Module name:	  findSkle
*		Purpose:		  It performs all the drawing skeleton and adding text in each individual objects.
*		Last update date: 09-06-2010
*
* [9]	Module name:	  setRenderAction
*		Purpose:		  Print unique object ID to the every object in the frame.
*		Last update date: 09-06-2010
*
******************************************************************************/


#pragma once

#include <sstream>
#include <fstream>
#include <highgui.h>
#include <vector>
#include "mvTracks.h"
#include "mvTrackObj.h"
#include "mvDhmm.h"


class Skeleton
{

public:
	virtual void setFont()=0;
	virtual void setCodebook(ifstream *codebook)=0;
	virtual void setDhmm(DHMM * dhm)=0;
	virtual vector<CvPoint2D32f> drawSkele(IplImage *grey, int x_, int y_, vector<CvPoint2D32f> vec, MvTrack *track, int frameNumber, TrackObj *trackObj)=0;
	virtual void sequenceGenerator(vector <CvPoint2D32f > points, int bar_x, int bar_y, MvTrack *track, int frameNumber,TrackObj *trackObj)=0;
	virtual double findMin( std::vector<double> minx)=0;
	virtual int  findMinPos(std::vector<double> minp)=0;
	virtual int findSkle(MvTracks *tracks,IplImage *bgs,int frameNumber,IplImage *)=0;
	virtual void setRenderAction(MvTracks *tracks,IplImage *originalImg)=0;

};
