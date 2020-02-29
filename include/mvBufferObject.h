/****************************************************************************
*
* mvBufferObject.h
*
* Purpose: Insted of frames, Buffer contain bufferObject. Each bufferObjects contain current frame's original image, background subtracted image,
*		   blobs available in that frame and current tracks.
*
* Author: Nihanth
*
*
* Class: BufferObject
* Modules:	BufferObject::setOriginalImage
*						::getOriginalImage
*						::setFrameNumber
*						::getFrameNumber
*						::setBGSImage
*						::getBGSImage
*						::setTracks
*						::getTracks
*						::setBlobs
*						::getBlobs
*
* 
* [1]	Module name:	  setOriginalImage
*		Purpose:		  Set the original image for current frame.
*		Last update date: 11-06-2010
* 
* [2]	Module name:	  getOriginalImage
*		Purpose:		  Get the original image.
*		Last update date: 11-06-2010
* 
* [3]	Module name:	  setFrameNumber
*		Purpose:		  Set the frame number.
*		Last update date: 11-06-2010
*
* [4]	Module name:	  getFrameNumber
*		Purpose:		  Number of frames, a buffer can hold.
*		Last update date: 11-06-2010
*
* [5]	Module name:	  setBGSImage
*		Purpose:		  Set the background subtracted image for current frame.
*		Last update date: 11-06-2010
* 
* [6]	Module name:	  getBGSImage
*		Purpose:		  Get the background subtracted image.
*		Last update date: 11-06-2010
* 
* [7]	Module name:	  setTracks
*		Purpose:		  Set tracks for current frame.
*		Last update date: 11-06-2010
*
* [8]	Module name:	  getTracks
*		Purpose:		  Get the tracks.
*		Last update date: 11-06-2010
*
* [9]	Module name:	  setBlobs
*		Purpose:		  Set blbos for current frame.
*		Last update date: 11-06-2010
*
* [10]	Module name:	  getBlobs
*		Purpose:		  Get the blobs.
*		Last update date: 11-06-2010
*
******************************************************************************/


#pragma once

#include "moveItBlob.h"

using namespace moveit;

class BufferObject
{

private:
	IplImage *originalImage;	// Original Image
	IplImage *bgsImage;			// Background Subtracted Image
	MvTracks *tracks;			// Tracks map
	MvBlobs *blobs;				// Blob map
	int frameNumber;			// Frame Number


public:
	BufferObject(void);
	~BufferObject(void);

	void setOriginalImage(IplImage *ori)
	{
		originalImage = ori;
	}

	IplImage* getOriginalImage()
	{
		return originalImage;
	}

	void setFrameNumber(int frame)
	{
		frameNumber = frame;
	}

	int getFrameNumber()
	{
		return frameNumber;
	}

	void setBGSImage(IplImage* tempImg)	
	{
		bgsImage = tempImg;
	}

	IplImage* getBGSImage()
	{
		return bgsImage;
	}

	MvTracks *getTracks()
	{
		return tracks;
	}

	void setTracks(MvTracks *track)
	{
		tracks=track;
	}

	MvBlobs* getBlobs()
	{
		return blobs;
	}

	void setBlobs(MvBlobs *blob)
	{
		blobs=blob;
	}

};