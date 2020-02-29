/****************************************************************************
*
* mvBgs.h
*
* Purpose: Base class for BGS algorithms.
*
* Author: Thusy, Nihanth, Sri
*
*
* Namespace: BackgroundSubtraction
* Class: Bgs
* Modules:	BackgroundSubtraction::Bgs::initalize
*									  ::initModel
*									  ::backgroundSubtract
*
* [1]	Module name:	  initalize
* 		Purpose:		  Initialize any data required by the BGS algorithm.
*		Last update date: 11-06-2010
* 
* [2]	Module name:	  initModel
*		Purpose:		  Initialize the background model.
*		Last update date: 11-06-2010
*
* [3]	Module name:	  backgroundSubtract
*		Purpose:		  Subtract the current frame from the background model and produce a binary foreground mask using low threshold value.
* 						  - Detect available blobs that is detect connected parts of a binary image.
* 						  - Label the connected parts of a binary image. 
*		Last update date: 11-06-2010
*
******************************************************************************/


#pragma once

#include "mvImage.h"
#include "mvBgsParams.h"
#include "moveItBlob.h"

using namespace moveit;


namespace BackgroundSubtraction
{

	class Bgs
	{

	public:
		static const int BACKGROUND = 0;		// define Background value to 0
		static const int FOREGROUND = 255;		// define Froeground value to 0

		virtual ~Bgs() {}

		virtual void initalize(const BgsParams& param) = 0;
		virtual void initModel(const RgbImage& data) = 0;
		virtual void backgroundSubtract(int frameNum, const RgbImage& data, BwImage& lowThresholdMask, IplImage *imgOut, MvBlobs &blobs) = 0;

	};

};


