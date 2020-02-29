/****************************************************************************
*
* mvInputFilter.h
*
* Purpose: Read frames from video file and put it into the cyclic buffer.
*
* Author: Nihanth
*
*
* Class: InputFilter
*
* Last update date: 08-06-2010
*
******************************************************************************/


#pragma once

#include "tbb/pipeline.h"
#include "mvBuffer.h"


class InputFilter: public tbb::filter
{

public:
    static const size_t numberofBuffer = 8;		// Number of cyclic buffer objects in the buffer.

    InputFilter( CvCapture* inputFile, bool volatile& end);
	~InputFilter(void);


private:
	bool volatile& endOfFile;		// To check and indicate the end of the video file.
	int frameNum;					// Frame Number.
    CvCapture* inputFile;			// Input file.
    size_t nextBuffer;				// Indicate the next buffer. 
    Buffer buffer[numberofBuffer];  // Cyclic buffer, which can hold only 8 buffer objects.
    void* operator()(void*);		// Operator overloading function is used for run InputFilter in pipeline. 
};
