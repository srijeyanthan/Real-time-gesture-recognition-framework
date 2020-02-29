/****************************************************************************
*
* mvInitializeParallel.h
*
* Purpose: This class used to handle PIPELINE
*
* Author: Nihanth
*
*
* Class: InitializeParallel
* Modules:	BufferObject::runPipeline
*
* [1]	Module name:	runPipeline
*		Purpose:		Create the pipeline and run. 
*		Last update date: 09-06-2010
*
******************************************************************************/


#pragma once

#include <highgui.h>
#include<fstream>
#include "mvDhmm.h"

class InitializeParallel
{

public:
	~InitializeParallel(void);
	int runPipeline(CvCapture* inputFile, byte show, DHMM * dhm, ifstream *mvFileCodebook);

};