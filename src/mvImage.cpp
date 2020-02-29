/****************************************************************************
*
* mvImage.cpp
*
* Purpose:  Implementation of mvImage.h
*			C++ wrapper for OpenCV IplImage which supports simple and efficient access to the image data
*
* Author: Thusy, Sri, Nihanth
*
*
* Class: ImageBase
*
* Last update date: 07-06-2010
*
******************************************************************************/


#include "mvImage.h"


ImageBase::~ImageBase()
{ 
	if(imgp != NULL && m_bReleaseMemory)
		cvReleaseImage(&imgp);
	imgp = NULL;	
}

