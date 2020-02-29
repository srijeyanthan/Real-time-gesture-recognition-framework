/****************************************************************************
*
* mvAdaptiveMedianBgs.h
*
* Purpose: Implementation of the simple Adaptive Median Background Subtraction Algorithm.
*		   Detect Blobs and label the connected parts of a binary image. 
*
* Author: Thusy, Nihanth, Sri
* 
* 
* Namespace: BackgroundSubtraction
* Class: AdaptiveMedianParams
* Modules: BackgroundSubtraction::AdaptiveMedianParams::samplingRate
* 	 										  		  ::mvLowThreshold
* 	 												  ::learningFrames
* 
* [1]	Module name:	  samplingRate
*		Purpose:		  Set and get the sample rate to Adaptive Median Algorithm.
*		Last update date: 23-06-2010
*
* [2]	Module name:	  mvLowThreshold
*		Purpose:		  Set and get the threshold vaue to Adaptive Median Algorithm.
*		Last update date: 23-06-2010
* 
* [3]	Module name:	  learningFrames
*		Purpose:		  Set and get the learning frames rate to Adaptive Median Algorithm.
*		Last update date: 23-06-2010
*
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
* [1]	Module name:	  find
*		Purpose:		  Find the parent of a given blob.
*		Reference:		  "Disjoint-set data structure", 7 july, 2010. [Online]. 
*						  Available: http://en.wikipedia.org/wiki/Disjoint-set_data_structure. 
*						  [Accessed: July 20, 2010].
*		Last update date: 23-06-2010
*
* [2]	Module name:	  merge
*		Purpose:		  Merge the same blobs.
*		Reference:		  "Disjoint-set data structure", 7 july, 2010. [Online].
*						  Available: http://en.wikipedia.org/wiki/Disjoint-set_data_structure. 
*						  [Accessed: July 20, 2010].
* 		Last update date: 23-06-2010
*
* [3]	Module name:	  update
*		Purpose:		  Update the background model.
*		Last update date: 23-06-2010
*
* [4]	Module name:	  makeSet
*		Purpose:		  Make the parent and rank of the union-find data estructure. 
*		Reference:		  "Disjoint-set data structure", 7 july, 2010. [Online]. 
*						  Available: http://en.wikipedia.org/wiki/Disjoint-set_data_structure. 
*						  [Accessed: July 20, 2010].
*		Last update date: 23-06-2010
*
* [5]	Module name:	  initModel
*		Purpose:		  Create initial background model.
*		Last update date: 23-06-2010
* 
* [6]	Module name:	  initalize
* 		Purpose:		  Initalize the parameters of Adaptive Median Background subtraction.
*		Last update date: 23-06-2010
* 
* [7]	Module name:	  subtractPixel
*		Purpose:		  Subtract pixels from current pixel with background model pixel and based on threshold value 
*						  detect whether current pixel is BACKGROUND or FOREGROUND.
*		Last update date: 23-06-2010
*
* [8]	Module name:	  shadowRemoval
*		Purpose:		  Remove shadow from video.
*		Algorithm:		  Shadow removal:
* 
*						  foreR - current pixel's R value
*						  foreG - current pixel's G value
*						  foreB - current pixel's B value
*						  backR - media pixel's R value
*						  backG - media pixel's G value
*						  backB - media pixel's B value
*						  calculate productR = foreR * backR;
*						  calculate productG = foreG * backG;
*						  calculate productB = foreB * backB;
*						  BD = (productR + productG + productB) / (productBackR + productBackG + productBackB);
*						  CD = sqrt (CDR * CDR + CDG * CDG + CDB * CDB);
* 
*						  If CD < 10.0 THEN
* 							If 0.5 < BD < 1.0 THEN 
* 								SHADOW
* 							Else if 1.0 < BD < 1.25 THEN 
* 								HIGHLIGHT
*						  Else FOREGROUND
*		Reference:		  T. Horprasert, D. Harwood and, L. S. Davis, A statistical approach for real-time robust background
*						  subtraction and shadow detection. In Proceedings of IEEE ICCV99 FRAME-RATE WORKSHOP, 1999.
*		Last update date: 23-06-2010
*		
* [9]	Module name:	  backgroundSubtract
*		Purpose:		  - Subtract pixels from current pixel with background model pixel.
* 						  - Detect available blobs that is detect connected parts of a binary image.
* 						  - Label the connected parts of a binary image. 
*		Last update date: 23-06-2010
*		Algorithm:		  Connected Component Labeling:
* 
*						  check pixel of first ROW and first COL (0, 0)
*						  IF it has white pixel THEN
* 							create a new blob
* 
* 						  check pixels of first ROW (0, c) except first COL where c=1,2,3...
* 						  FOR every pixels in the first ROW (0, c) except first COL where c=1,2,3...
* 							IF it has white pixel (0, c) THEN
* 								IF previous COL pixel (0, c-1) has white pixel THEN
* 									both are same blob
* 						 		ELSE Create new Blob
* 						 
* 						  FOR every ROW (r,c) other than first where r=1,2,3...
* 						 	check first COL (r, 0) where r=1,2,3...
* 						 	IF it has white pixel (r, 0) THEN
* 						 		IF pixel above current COL (r-1,0) has white pixel THEN
* 						 			both are same blob
* 						 		ELSE Create new Blob
* 						 
* 						  check every pixel in ROW r except first COL (r, c) where c=1,2,3...
* 						  FOR every pixels (r, c) except first COL pixel where c=1,2,3...
* 							IF it has white pixel (r, c) THEN
* 						 		IF pixel above current COL (r-1, c) has white pixel THEN
* 						 			both are same blob
* 						 			IF previous COL pixel (r,c-1) has white pixel and, both previous and current pixels	are different object THEN
*						 				merge both blob
*						 		ELSE IF previous COL pixel (r, c-1) has white pixel THEN
*						 			both are same blob
*						 		ELSE create new blob
* 
*		Reference:		  Joseph C.N., Kokulakumaran S., Srijeyanthan K., and Thusyanthan A., "moveIt," Final year report, 2010.
*		Last update date: 23-06-2010
*
******************************************************************************/


#pragma once

#include "mvBgs.h"


namespace BackgroundSubtraction
{
	//Parameters of Adaptive Median BGS algorithm
	class AdaptiveMedianParams : public BgsParams
	{

	private:
		unsigned char lowThresholdValue;// Threshold valus
		int sampleRate;					// Sampling rate
		int learnFrame;					// Learning rate


	public:
		unsigned char &mvLowThreshold() 
		{ 
			return lowThresholdValue; 
		}

		int &samplingRate() 
		{ 
			return sampleRate; 
		}

		int &learningFrames() 
		{ 
			return learnFrame; 
		}

	};


	//Adaptive Median BGS algorithm
	class AdaptiveMedianBGS : public Bgs 
	{

	private:
		int numPixels;
		int imgInWidth;
		int imgInHeight;
		int imgOutWidth;
		int imgOutHeight;
		CvSize frameSize;
		AdaptiveMedianParams params;
		RgbImage median;
		void subtractPixel(int r, int c, const RgbPixel& pixel, unsigned char& lowThreshold, int shadowImage);


	public:
		AdaptiveMedianBGS(){}
		virtual ~AdaptiveMedianBGS() {}

		void initalize(const BgsParams& param);
		void initModel(const RgbImage& data);
		void backgroundSubtract(int frameNum, const RgbImage& data, BwImage& lowThresholdMask, IplImage *imgOut, MvBlobs &blobs);	
		void merge(MvBlob *blob1, MvBlob *blob2);
		void makeSet(MvBlob *blob);
		MvBlob *find(MvBlob *blob);
		void update(int frameNum, const RgbImage& data,  const BwImage& updateMask);
		void shadowRemoval(const RgbImage& data, int i, int j, unsigned char &lowThreshold, BwImage& lowThresholdMask);

	};

};

