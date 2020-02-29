/****************************************************************************
*
* mvOutputFilter.h
*
* Purpose: Put the displayObjects into the queue sequently. Inorder to display images sequentialy, displayObjects are put it into the queue.
*
* Author: Sri,Nihanth
*
*
* Class: OutputFilter
*
* Last update date: 23-06-2010
*
******************************************************************************/


#pragma once

#include "mvTransformFilterSkele.h"
#include <iostream>
#include <vector>

using namespace std;

class OutputFilter: public tbb::filter
{

private:
	IplImage *txtImage;							// Contain text Image
	CvFont *font;								// Font style of text
	IplImage *blackbgs;							// Containg background subtracted Image
	concurrent_queue<DisplayObject *> *queue;	// This queue conatain Images that need to display. Using this Image can display sequentially. 
	int pos;									// Position
	bool checkFirstTen;
	int y;
	vector<string> *mvVector;


public:
	OutputFilter(concurrent_queue<DisplayObject *> *que);
	~OutputFilter(void);

	/*override*/void* operator()( void* item );	// Operator overloading function is used for run pipeline. 
	
};
