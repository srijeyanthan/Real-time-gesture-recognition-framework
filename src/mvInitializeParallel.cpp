/****************************************************************************
*
* mvInitializeParallel.cpp
*
* Purpose:  Implementation of mvInitializeParallel.h
*			This class used to handle PIPELINE.
*
* Author: Nihanth
*
*
* Class: InitializeParallel
* Modules:	InitializeParallel::runPipeline
*
* Last update date: 23-06-2010
*
******************************************************************************/


#include "mvInitializeParallel.h"
#include "mvOutputFilter.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/pipeline.h"
#include "tbb/tick_count.h"

using namespace tbb;


InitializeParallel::~InitializeParallel(void)
{}


//Create the pipeline and execute. 
int InitializeParallel::runPipeline(CvCapture* inputFile,byte show,DHMM * dhmm,ifstream *mvFileCodebook)
{
	try
	{
		tbb::task_scheduler_init init_parallel(1);
		
		if(inputFile == NULL)
		{
			return 0;
		}

		// Retrieve information about AVI file
		cvQueryFrame(inputFile); 
		int width	= (int) cvGetCaptureProperty(inputFile, CV_CAP_PROP_FRAME_WIDTH);
		int height = (int) cvGetCaptureProperty(inputFile, CV_CAP_PROP_FRAME_HEIGHT);
		int fps = (int) cvGetCaptureProperty(inputFile, CV_CAP_PROP_FPS);
		int numFrames = (unsigned int) cvGetCaptureProperty(inputFile,  CV_CAP_PROP_FRAME_COUNT);

		if(width!=320 || height!=240)		//Wrong video parameter
			return 1;

		if(fps>30)							//Wrong fps
			return 2;

		// Setup masks to hold results of low thresholding
		BwImage lowThresholdMask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
		lowThresholdMask.Ptr()->origin = IPL_ORIGIN_TL;

		// Setup buffer to hold individual frames from video stream
		RgbImage frame_data;
		frame_data.ReleaseMemory(false);	// AVI frame data is released by with the AVI capture device

		BackgroundSubtraction::AdaptiveMedianParams params;
		params.setFrameSize(width, height);

		params.mvLowThreshold() = 40;
		params.samplingRate() = 7;
		params.learningFrames() = 30;

		BackgroundSubtraction::AdaptiveMedianBGS *bgs = new BackgroundSubtraction::AdaptiveMedianBGS;
		bgs->initalize(params);
		cvGrabFrame(inputFile);
		frame_data = cvRetrieveFrame(inputFile); 
		IplImage *initial=cvCloneImage(frame_data.Ptr());
		bgs->initModel(frame_data); 
		concurrent_queue<DisplayObject *> *queue=new concurrent_queue<DisplayObject *>;
		volatile bool endOfFile = false; 
		Thread *thread=new Thread(endOfFile);
		thread->setParam(queue,width,height,fps,numFrames);

		tbb::tbb_thread t(*thread); 
		tbb::pipeline pipeline;				// Create Pipeline

		// Create file-reading and writing stage, and add it to the pipeline
		InputFilter *input_filter=new InputFilter( inputFile,endOfFile);
		pipeline.add_filter( *input_filter );

		// Create background subtraction and blob detectoin stage and add it to the pipeline
		TransformFilter *transform_filter= new TransformFilter; 
		transform_filter->setVideoParameter(width, height, bgs);
		pipeline.add_filter( *transform_filter );

		// Create track stage and add it to the pipeline
		TransformFilterTracks *tracks_filter=new TransformFilterTracks(frame_data.Ptr());
		pipeline.add_filter(*tracks_filter);

		// Create skeletoniztion stage and add it to the pipeline
		TransformFilterSkele *skeleton_filter=new TransformFilterSkele;
		skeleton_filter->setCodebook(mvFileCodebook);
		skeleton_filter->setDhmm(dhmm);
		pipeline.add_filter(*skeleton_filter);

		// Create display stage and add it to the pipeline
		OutputFilter *output_filter=new OutputFilter(queue);
		pipeline.add_filter(*output_filter);

		// Execute the pipeline
		pipeline.run( InputFilter::numberofBuffer );
		pipeline.clear();

		t.join();

		queue->clear();
		frame_data.Clear();
		lowThresholdMask.Clear();

		delete thread;
		delete output_filter;
		delete skeleton_filter;
		delete transform_filter;
		delete tracks_filter;

		if(init_parallel.is_active())
			init_parallel.terminate();

	} catch(int x)	// If there is any errors
	{
		return x;
	} catch(...)
	{
		return 5;
	}

	return -1;		// If success
}


