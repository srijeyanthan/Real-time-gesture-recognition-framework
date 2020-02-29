/****************************************************************************
*
* mvOutputFilter.cpp
*
* Purpose:  Implementation of mvOutputFilter.h
*			Fifth filter to the pipeline.
*			Put the displayObjects into the queue sequently. Inorder to display images sequentialy, displayObjects are put it into the queue.
*
* Author: Nihanth
*
*
* Class: OutputFilter
*
* Last update date: 23-06-2010
*
******************************************************************************/


#include "mvOutputFilter.h"

using namespace tbb;


OutputFilter::OutputFilter(concurrent_queue<DisplayObject *> *que) : 
tbb::filter(serial_in_order), 
queue(que)
{
	blackbgs = cvLoadImage("images/blackbgs.png");
	txtImage=cvCloneImage(blackbgs);
	font=new CvFont;
	cvInitFont(font, CV_FONT_HERSHEY_DUPLEX, .5, .5, 0, 1, CV_AA);
	mvVector = new vector<string>(10);
	pos=0;
	checkFirstTen=false;
}


OutputFilter::~OutputFilter(void)
{}


// Operator overloading function is used for run pipeline. 
void* OutputFilter::operator()( void* item ) 
{
	Buffer& b = *static_cast<Buffer*>(item);

	for( BufferObject *s=b.begin(); s!=b.end(); ++s ) 
	{
		DisplayObject *display = new DisplayObject;
		IplImage *original=s->getOriginalImage();
		txtImage=cvCloneImage(blackbgs);

		for (MvTracks::const_iterator it=s->getTracks()->begin(); it!=s->getTracks()->end(); ++it)
		{
			if (!it->second->inactive)
			{
				stringstream buffer;

				if((it->second->action).compare("")==0){
					buffer<<"";
				} else
				{
					buffer<<"H"<<it->second->id<<" -> "<<it->second->action;
					string txt=buffer.str().c_str();
					mvVector->at(pos%10)= txt;
					pos++;
					checkFirstTen=true;

					if(pos>=mvVector->size())
					{
						pos=0;
					}
				}

				if(it->second->action.compare("Bag drop!")==0 || it->second->action.compare("asking help!")==0 || it->second->action.compare("falling")==0 || it->second->detectId>0 && it->second->action.compare("")==0)
				{	
					cvRectangle(original, cvPoint(it->second->minx, it->second->miny), cvPoint(it->second->maxx, it->second->maxy), CV_RGB(255.,0.,0.),3);
				}
			}

		}

		y=30;
		int position=pos;
		int numberOfElement=0;				// Maximum numberOfElement is 10

		if(checkFirstTen)
		{
			do
			{
				if(position>=10)
				{
					position=0;
				}

				if((mvVector->at(position)).compare("")!=0)
				{
					cvPutText(txtImage, mvVector->at(position).c_str(), cvPoint(20,y), font, CV_RGB(0.,255.,0.));
					y+=20;
				}

				position++;
				numberOfElement++;
			}while(position!=pos && numberOfElement<=9);
		}

		display->oriImage=original;
		display->txtImage=txtImage;
		queue->push(display);
		IplImage *bgs=s->getBGSImage();
		cvReleaseImage(&bgs);
	}

	return NULL;
}

