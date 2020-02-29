/****************************************************************************
*
* mvBufferObject.cpp
*
* Purpose: Implementation of mvBufferObject.h
*		   Insted of frames, Buffer contain bufferObject. Each bufferObjects contain current frame's original image, background subtracted image,
*		   blobs available in that frame and current tracks.
*
* Author: Nihanth
*
*
* Class: BufferObject
*
* Last update date: 11-06-2010
*
******************************************************************************/


#include "mvBufferObject.h"


BufferObject::BufferObject()
{
	originalImage=NULL;
	bgsImage=NULL;
}


BufferObject::~BufferObject()
{
}