/****************************************************************************
*
* mvTransformFilter.cpp
*
* Purpose:  Implementation of mvTransformFilter.h
*			Second filter to the pipeline.
*			Filter that detect Blobs from background subtracted images.
*
* Author: Nihanth
*
*
* Class: TransformFilter
* Modules:	TransformFilter::setVideoParameter
*
* Last update date: 23-06-2010
*
******************************************************************************/


#include "mvTransformFilter.h"
using namespace std;


TransformFilter::TransformFilter() : 
tbb::filter(serial_in_order) 
{}  


TransformFilter::~TransformFilter(void)
{
}


// Operator overloading function is used for run TransformFilter in pipeline. 
void* TransformFilter::operator()( void* item ) 
{
	Buffer& b = *static_cast<Buffer*>(item);

	BufferObject *buffer;
	BwImage lowThresholdMask = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 1);
	IplImage *threeChannelFrame = cvCreateImage(cvSize(width,height), 8, 3);
	IplImage *frame = cvCreateImage(cvSize(width,height), 8, 3);
	for( buffer=b.begin(); buffer!=b.end(); ++buffer ) {

		IplImage *originalImage = cvCloneImage (buffer->getOriginalImage());
		IplImage *ori=cvCloneImage (buffer->getOriginalImage());
		IplImage *labelImg = cvCreateImage(cvSize(width,height), IPL_DEPTH_LABEL, 1);

		MvBlobs *blobs=new MvBlobs;
		cvConvertImage(lowThresholdMask.Ptr(), threeChannelFrame);
		cvConvertScale(threeChannelFrame, frame, 1, 0);

		// Call background subtraction
		bgs->backgroundSubtract(buffer->getFrameNumber(), originalImage, lowThresholdMask,labelImg, *blobs);
		IplImage *chB=cvCloneImage(lowThresholdMask.Ptr());
		filterByArea(*blobs, 335, 50000,chB);		// Remove blobs which is less than 335 pixel and and greater than 50000 pixel
		mergeBlobs(*blobs);							// Merge nearest blobs

		IplImage *backgroundSubImg=cvCloneImage(chB);
		buffer->setBGSImage(backgroundSubImg);
		buffer->setBlobs(blobs);
		cvReleaseImage(&labelImg);
		cvReleaseImage(&chB);
		bgs->update(buffer->getFrameNumber(),ori,lowThresholdMask);	// update the background model
		lowThresholdMask.Clear();

	}

	return &b;  
}


