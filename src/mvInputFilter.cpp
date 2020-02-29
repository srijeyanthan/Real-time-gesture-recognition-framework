/****************************************************************************
*
* mvInputFilter.cpp
*
* Purpose:  Implementation of mvInputFilter.h
*			First filter to the pipeline.
*			Read frames from video file and put it into the cyclic buffer.
*
* Author: Nihanth
*
*
* Class: InputFilter
*
* Last update date: 08-06-2010
*
******************************************************************************/


#include "mvInputFilter.h"
#include "mvThread.h"


InputFilter::InputFilter( CvCapture* inputFile , bool volatile& end) : 
tbb::filter( serial_in_order),
nextBuffer(0),
inputFile(inputFile),
endOfFile(end)
{
	frameNum=0;
}


InputFilter::~InputFilter(void)
{}


// Operator overloading function is used for run InputFilter in pipeline. 
void* InputFilter::operator()(void*) 
{
    Buffer& b = buffer[nextBuffer];
    nextBuffer = (nextBuffer+1) % numberofBuffer;
	size_t i=0;

	for(; i < b.maxSize(); ++i)
	{
		if(!cvGrabFrame(inputFile))
		{   
			endOfFile=true;
			return NULL;
		}

		IplImage *p=cvCloneImage(cvRetrieveFrame(inputFile));
		b.begin()[i].setOriginalImage(p);
		b.begin()[i].setFrameNumber(frameNum++);
	}

	b.setEnd(b.begin()+i);
	return &b;
}



