/****************************************************************************
*
* mvTransformFilter.h
*
* Purpose: Filter that detect Blobs from background subtracted images.
*
* Author: Nihanth
*
*
* Class: TransformFilter
* Modules:	TransformFilter::setVideoParameter
*
* [1]	Module name:	setVideoParameter
*		Purpose:		Set the video parameter. 
*		Last update date:
*
* Last update date: 08-06-2010
*
******************************************************************************/


#pragma once

#include "mvInputFilter.h"
#include "mvAdaptiveMedianBgs.h"

using namespace BackgroundSubtraction;


class TransformFilter: public tbb::filter
{

private:
	int width;				// Width of frame
	int height;				// Height of frame
	AdaptiveMedianBGS *bgs;


public:
	TransformFilter();
	~TransformFilter(void);

    /*override*/void* operator()( void* item );	// Operator overloading function is used for run TransformFilter in pipeline. 
	void setVideoParameter(int W, int H, AdaptiveMedianBGS *b){
		width=W;
		height=H;
		bgs=b;
	}

	
};
