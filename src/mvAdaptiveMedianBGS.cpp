/****************************************************************************
*
* mvAdaptiveMedianBGS.cpp
*
* Purpose: Implementation of mvAdaptiveMedianBgs.h
*		   Implementation of the simple Adaptive Median Background Subtraction Algorithm.
*		   Detect Blobs and label the connected parts of a binary image. 
*
* Author: Thusy, Nihanth, Sri 
*
*
* Namespace: BackgroundSubtraction
* Class: AdaptiveMedianBGS
* Modules: BackgroundSubtraction::AdaptiveMedianBGS::find
* 												   ::merge
* 												   ::update
* 												   ::makeSet
* 												   ::initModel
* 												   ::initalize
* 												   ::subtractPixel
* 												   ::shadowRemoval
* 												   ::backgroundSubtract
*
* Last update date: 23-06-2010
*
******************************************************************************/


#include <iostream>
#include <stdlib.h>
#include <cmath>

#include "mvAdaptiveMedianBgs.h"

using namespace BackgroundSubtraction;


// Find the parent of a given blob
MvBlob *AdaptiveMedianBGS::find(MvBlob *blob)
{
	if (!blob->_parent) 
	{
		return blob;
	} else
	{
		blob->_parent=find(blob->_parent);
		return blob->_parent;
	}
}


// Merge the same blobs
void AdaptiveMedianBGS::merge(MvBlob *blob1, MvBlob *blob2)
{
	MvBlob *xRoot=find(blob1);
	MvBlob *yRoot=find(blob2);

	if (xRoot->_rank > yRoot->_rank)
		yRoot->_parent=xRoot;
	else if (xRoot->_rank < yRoot->_rank)
		xRoot->_parent=yRoot;
	else if (xRoot!=yRoot)
	{
		yRoot->_parent=xRoot;
		xRoot->_rank+=1;
	}
}


// Initalize the parameters of adaptive median background subtraction
void AdaptiveMedianBGS::initalize(const BgsParams& param)
{
	params = (AdaptiveMedianParams&)param;
	median = cvCreateImage(cvSize(params.mvWidth(), params.mvHeight()), IPL_DEPTH_8U, 3);
	cvSet(median.Ptr(), CV_RGB(BACKGROUND, BACKGROUND, BACKGROUND));
}


// Create initial background model
void AdaptiveMedianBGS::initModel(const RgbImage& data)
{
	numPixels=0;
	frameSize = cvGetSize(data.Ptr());
	imgOutWidth = imgInWidth = data.Ptr()->width;
	imgOutHeight = imgInHeight = data.Ptr()->height;

	// Initialize the background model
	for (unsigned int r = 0; r < params.mvHeight(); ++r)
	{
		for(unsigned int c = 0; c < params.mvWidth(); ++c)
		{
			median(r,c) = data(r,c);
		}
	}
}


// Perform background subtraction
void AdaptiveMedianBGS::subtractPixel(int r, int c, const RgbPixel& pixel, unsigned char& lowThreshold, int shadowImage)
{
	lowThreshold  = FOREGROUND;

	int diffR = abs(pixel(0) - median(r,c,0));
	int diffG = abs(pixel(1) - median(r,c,1));
	int diffB = abs(pixel(2) - median(r,c,2));

	if(diffR  !=0 || diffG !=0 || diffB !=0)
	{
		diffR =diffR;
		diffG=diffG;
		diffB=diffB;
	}

	if(diffR <= params.mvLowThreshold() && diffG <= params.mvLowThreshold() &&  diffB <= params.mvLowThreshold())
	{
		lowThreshold = BACKGROUND;
	}

	// Detect shadow and remove
	if(shadowImage ==255){
		lowThreshold = BACKGROUND;
	}
}


// Calculate shadow 
void AdaptiveMedianBGS::shadowRemoval(const RgbImage& data, int i, int j, unsigned char &lowThreshold, BwImage& lowThresholdMask)
{
	float backR =  median(i,j,0);
	float backG = median(i,j,1);
	float backB = median(i,j,2);

	float foreR =  data(i,j,0);
	float foreG =  data(i,j,0);
	float foreB =  data(i,j,0);

	float productR = foreR * backR;
	float productG = foreG * backG;
	float productB = foreB * backB;

	float productBackR = backR * backR;
	float productBackG = backG * backG;
	float productBackB = backB * backB;

	float BD = (productR + productG + productB) / (productBackR + productBackG + productBackB);

	float CDR = foreR - BD * backR;
	float CDG = foreG - BD * backG;
	float CDB = foreB - BD * backB;

	float CDSquared = CDR * CDR + CDG * CDG + CDB * CDB;
	int shadowImages = CDSquared < 70.0f && BD < 1.f && BD > 0.3f ? 255  : 0;

	// Prform background subtraction
	subtractPixel(i, j, data(i,j), lowThreshold, shadowImages);

	// Setup silhouette mask
	lowThresholdMask(i,j) = lowThreshold;
}


// Make the parent and rank of the union-find data estructure. 
void AdaptiveMedianBGS::makeSet(MvBlob *blob)
{
	blob->_parent=NULL;
	blob->_rank=0;
}


// Perform background subtraction, detect blob and label the connected parts of a binary image. All this done together to increase the runtime performance
void AdaptiveMedianBGS::backgroundSubtract(int frameNum, const RgbImage& data, BwImage& lowThresholdMask, IplImage *imgOut, MvBlobs &blobs)
{
	cvSetZero(imgOut);
	MvLabel label=0;
	releaseBlobs(blobs);
	unsigned char lowThreshold;
	int numPixels=0;
	int stepOut = imgOut->widthStep / (imgOut->depth / 8);
	int imgInWidth = imgOut->width;
	int imgInHeight = imgOut->height;
	int imgOutWidth = imgOut->width;
	int imgOutHeight = imgOut->height;
	int imgOutOffset = 0;

	MvLabel *imgDataOut = (MvLabel *)imgOut->imageData + imgOutOffset;
	shadowRemoval(data, 0, 0, lowThreshold, lowThresholdMask);			// Shadow removal

	/* Blob detection */

	// Check first pixel (0, 0) for blob detection
	if (lowThreshold)
	{
		label++;
		MvBlob *blob=new MvBlob;
		makeSet(blob);
		blob->label=label;
		blob->area=1;
		blob->minx=0; 
		blob->maxx=0;
		blob->miny=0; 
		blob->maxy=0;
		blob->m10=0; 
		blob->m01=0;
		blobs.insert(MvLabelBlob(label, blob));
		imgDataOut[0]=label;
	}

	// Check first row (c, 0) for blob detection
	for (unsigned int c=1;c<(unsigned int)imgInWidth;c++)
	{
		shadowRemoval(data, 0,c,lowThreshold,lowThresholdMask);

		if (lowThreshold)
		{
			numPixels++;

			if (imgDataOut[c-1])
			{
				MvBlob *blob=blobs[imgDataOut[c-1]];
				blob->area+=1;
				blob->maxx=MAX(blob->maxx,c);
				blob->m10+=c; blob->m01+=0;
				imgDataOut[c]=imgDataOut[c-1];
			} else
			{
				label++;
				MvBlob *blob=new MvBlob;
				makeSet(blob);
				blob->label=label;
				blob->area=1;
				blob->minx=c; blob->maxx=c;
				blob->miny=0; blob->maxy=0;
				blob->m10=c; blob->m01=0;
				blobs.insert(MvLabelBlob(label,blob));
				imgDataOut[c]=label;
			}
		}
	}

	MvLabel *lastRowOut=(MvLabel *)imgOut->imageData + imgOutOffset;
	imgDataOut+=stepOut;

	// Check other than first row (r, c)  [r!=0]  for blob detection
	for (unsigned int r=1;r<(unsigned int)imgInHeight;r++,lastRowOut+=stepOut,imgDataOut+=stepOut)
	{
		shadowRemoval(data, r,0,lowThreshold,lowThresholdMask);

		// Check fist pixel (r,0)  for blob detection
		if (lowThreshold)
		{
			numPixels++;
			if (lastRowOut[0])
			{
				MvBlob *blob=blobs[lastRowOut[0]];
				blob->area+=1;
				blob->maxy=MAX(blob->maxy,r);
				blob->m10+=0; blob->m01+=r;
				imgDataOut[0]=lastRowOut[0];
			} else
			{
				label++;
				MvBlob *blob=new MvBlob;
				makeSet(blob);
				blob->label=label;
				blob->area=1;
				blob->minx=0; blob->maxx=0;
				blob->miny=r; blob->maxy=r;
				blob->m10=0; blob->m01=r;
				blobs.insert(MvLabelBlob(label,blob));
				imgDataOut[0]=label;
			}
		}

		// Check other than first pixel (r,c)  [c!=0]  for blob detection
		for (unsigned int c=1;c<(unsigned int)imgInWidth;c++)
		{
			shadowRemoval(data, r,c,lowThreshold,lowThresholdMask);

			if (lowThreshold)
			{
				numPixels++;

				if (lastRowOut[c])
				{
					MvBlob *blob=blobs[lastRowOut[c]];
					blob->area+=1;
					blob->maxy=MAX(blob->maxy,r);
					blob->m10+=c; blob->m01+=r;
					imgDataOut[c]=lastRowOut[c];

					if ((imgDataOut[c-1])&&(imgDataOut[c]!=imgDataOut[c-1]))
					{
						MvBlob *blob1=blobs[imgDataOut[c]];
						MvBlob *blob2=blobs[imgDataOut[c-1]];
						merge(blob1,blob2);
					}
				} else if (imgDataOut[c-1])
				{
					MvBlob *blob=blobs[imgDataOut[c-1]];
					blob->area+=1;
					blob->maxx=MAX(blob->maxx,c);
					blob->m10+=c; blob->m01+=r;
					imgDataOut[c]=imgDataOut[c-1];
				} else
				{
					label++;
					MvBlob *blob=new MvBlob;
					makeSet(blob);
					blob->label=label;
					blob->area=1;
					blob->minx=c; blob->maxx=c;
					blob->miny=r; blob->maxy=r;
					blob->m10=c; blob->m01=r;
					blobs.insert(MvLabelBlob(label,blob));
					imgDataOut[c]=label;
				}
			}
		}
	}

	unsigned int labelSize=blobs.size();
	MvLabel *luLabels=new MvLabel[labelSize+1];
	luLabels[0]=0;

	for (MvBlobs::iterator it=blobs.begin();it!=blobs.end();++it)
	{
		MvBlob *blob1=(*it).second;
		MvBlob *blob2=find(blob1);

		if (blob1!=blob2)
		{
			blob2->area+=blob1->area;
			blob2->minx=MIN(blob2->minx,blob1->minx); blob2->maxx=MAX(blob2->maxx,blob1->maxx);
			blob2->miny=MIN(blob2->miny,blob1->miny); blob2->maxy=MAX(blob2->maxy,blob1->maxy);
			blob2->m10+=blob1->m10; blob2->m01+=blob1->m01;
		}
		luLabels[(*it).first]=blob2->label;
	}

	imgDataOut=(MvLabel *)imgOut->imageData + imgOutOffset;

	for (int r=0;r<imgOutHeight;r++,imgDataOut+=stepOut)
		for (int c=0;c<imgOutWidth;c++)
			imgDataOut[c]=luLabels[imgDataOut[c]];

	delete [] luLabels;
	MvBlobs::iterator it=blobs.begin();

	while (it!=blobs.end())
	{
		MvBlob *blob=(*it).second;
		if (blob->_parent)
		{
			delete blob;
			MvBlobs::iterator tmp=it;
			++it;
			blobs.erase(tmp);
		} else
		{
			centroid((*it).second); 
			++it;
		}
	}
}


// Update the background model
void AdaptiveMedianBGS::update(int frameNum, const RgbImage& data,  const BwImage& updateMask)
{
	if(frameNum % params.samplingRate() == 1)
	{
		for (unsigned int r = 0; r < params.mvHeight(); ++r)
		{
			for(unsigned int c = 0; c < params.mvWidth(); ++c)
			{
				// perform conditional updating only if we are passed the learning phase
				if(updateMask(r,c) == BACKGROUND || frameNum < params.learningFrames())
				{
					for(int ch = 0; ch < NUM_CHANNELS; ++ch)
					{
						if(data(r,c,ch) > median(r,c,ch))
						{
							median(r,c,ch)++;
						} else if(data(r,c,ch) < median(r,c,ch))
						{
							median(r,c,ch)--;
						}
					}
				}
			}
		}
	}
}