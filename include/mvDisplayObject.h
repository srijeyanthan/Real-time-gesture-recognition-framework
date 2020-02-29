/****************************************************************************
*
* mvDisplayObject.h
*
* Purpose: This class contain images that need to display.
*
* Author: Nihanth
*
*
* Class: DisplayObject
*
* Last update date: 23-06-2010
*
******************************************************************************/


#pragma once

#include <highgui.h>
#include <cv.h>

class DisplayObject
{

public:
	IplImage *oriImage;			// Original image
	IplImage *txtImage;			// Image that contain only action

};