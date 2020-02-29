/****************************************************************************
*
* mvThread.h
*
* Purpose: Get the image from queue and display sequentially.
*		   Display every frame nearly every 40ms.
*
* Author: Nihanth
*
*
* Class: Thread
* Modules:	Thread::setParam
*
* [1]	Module name:	setParam
*		Purpose:		Set the video parameter. 
*		Last update date: 23-06-2010
*
******************************************************************************/


#pragma once

#include <tbb/tbb_Thread.h>
#include "tbb/concurrent_queue.h"
#include "mvBufferObject.h"
#include "tbb/tick_count.h"
#include "mvDisplayObject.h"

using namespace tbb;


class Thread
{

private:
	concurrent_queue<DisplayObject *> *queue;	// queue which is used to store images
	bool emptyQueue;							// check whether queue is empty or not
	int width;									// width of a frame
	int height;									// height of a frame
	bool stop;									// check whether file is end or not
	bool volatile& endOfFile;					// end of the file
	IplImage *correspond;						// Display Image
	CvFont *font;								// font
	int frameNumbers;							// video's frame Numbers
	int fps;									// frame per seconds of video
	tbb::tick_count prevTime;					// Time when previous frame is displayed.
	tbb::tick_count currentTime;				// Current time


public:
	void setParam(concurrent_queue<DisplayObject *> *que, int w, int h,int framePerSeconds, int frames)
	{
		queue=que;
		width=w;
		height=h;
		fps=framePerSeconds;
		frameNumbers=frames;

	}
	~Thread(void);
	Thread(bool volatile& end);
	void operator ()();

};


