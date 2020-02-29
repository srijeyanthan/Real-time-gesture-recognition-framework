/****************************************************************************
*
* mvBuffer.cpp
*
* Purpose: Implementation of mvBuffer.h
*		   Cyclic buffer that holds set of bufferObjects, each bufferObjects holds 10 frames.
*
* Author: Nihanth
*
*
* Class: Buffer
*
* Last update date: 17-06-2010
*
******************************************************************************/


#include "mvBuffer.h"


Buffer::Buffer(void)
{
}

Buffer::~Buffer(void)
{
	delete []&storage;
	delete mvEnd;
}
