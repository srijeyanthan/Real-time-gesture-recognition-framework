/****************************************************************************
*
* mvThread.cpp
*
* Purpose:  Implementation of mvThread.h
*			Get the image from queue and display sequentially.
*
* Author: Sri
*
*
* Class: Thread
* Modules:	Thread::setParam
*
* Last update date: 23-06-2010
*
******************************************************************************/

#include "mvThread.h"


Thread::Thread(bool volatile& end) : endOfFile(end)
{
	stop=false;
	emptyQueue=true;
	font=new CvFont;
	cvInitFont(font, CV_FONT_HERSHEY_DUPLEX , .4, .4, 0.2, .8, CV_AA);
}


Thread::~Thread()
{}


// concurrently execute pipeline and display the frames.
void Thread::operator ()()
{
	cvNamedWindow("moveIt", CV_WINDOW_AUTOSIZE);		// Name display Window

	correspond = cvLoadImage("images/background.png");	// Background Image

	cvShowImage("moveIt", correspond);					// Display GUI window
	cvWaitKey(1);

	prevTime=tbb::tick_count::now();

	float total_seconds=0., ms=0.00, sec=0.;
	int seconds=0,total_minutes = 0, minutes = 0, total_hours = 0, hours = 0, count = 1, numberOfFrames=0, xx=0;
	char inputFPS[25], inputTime[25], outputFPS[25], outputTime[25];

	// Calculate Input video Time
	double inputTimeSeconds=frameNumbers/fps;
	seconds=(int)inputTimeSeconds%60;
	ms = modf(inputTimeSeconds,&sec);
	total_minutes = (int)inputTimeSeconds / 60;
	minutes = total_minutes % 60;
	total_hours = total_minutes / 60;
	hours = total_hours % 24;

	// Display Input video's fps and Time
	sprintf(inputFPS, "     fps: %d",fps);										
	sprintf(inputTime,"I/p Time: %d:%d:%d:%1.f",hours,minutes,seconds,ms*100);

	while (!stop )				// Untill end of file
	{  
		DisplayObject *temp;

		if(queue->try_pop(temp))
		{
			numberOfFrames++;
			emptyQueue=false;
			cvSetImageROI( correspond, cvRect( 10, 10, width, height ) );
			cvCopy( temp->oriImage, correspond );
			currentTime=tbb::tick_count::now();
			total_seconds=(float)(currentTime-prevTime).seconds();
			seconds=(int)total_seconds%60;
			ms = modf(total_seconds,&sec);
			total_minutes = (int)total_seconds / 60;
			minutes = total_minutes % 60;
			total_hours = total_minutes / 60;
			hours = total_hours % 24;

			if(seconds==count){
				xx=numberOfFrames;
				numberOfFrames=1;

				count++;
			}

			// Output Time
			sprintf(outputTime,"O/p Time: %d:%d:%d:%2.f",hours,minutes,seconds,ms*100);
			sprintf(outputFPS, "      fps: %d",xx);

			cvPutText(correspond,inputTime,cvPoint(width -130,height - 20),font, CV_RGB(0.,255.,255.));
			cvPutText(correspond,inputFPS,cvPoint(width -130,height - 5),font, CV_RGB(0.,255.,255.));
			cvPutText(correspond,outputTime,cvPoint(5,height - 20),font, CV_RGB(0.,255.,255.));
			cvPutText(correspond,outputFPS,cvPoint(5,height - 5),font, CV_RGB(0.,255.,255.));

			cvResetImageROI( correspond );
			cvSetImageROI( correspond, cvRect( 340, 10, width, height ) );
			cvCopy( temp->txtImage, correspond );
			cvResetImageROI( correspond );
			cvReleaseImage(&temp->oriImage);
			cvReleaseImage(&temp->txtImage);
			cvShowImage("moveIt", correspond);
			cvWaitKey(40);			// Display every frame in 40ms

		}else if (!emptyQueue&&endOfFile)
		{
			stop=true;
		}

	}  
	cvDestroyWindow("moveIt");
}

