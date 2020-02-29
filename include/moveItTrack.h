/****************************************************************************
*
* moveItTrack.h
*
* Purpose: Base class for Tracking algorithms.
*
* Author: Sri,Nihanth
* 
* 
* Class:   mvTrack
* Modules: mvTrack::findPairs
*				  ::tracking
*				  ::renderTracks
*				  ::naiveNearestNeighbor
*				  ::compareSURFDescriptors
*				  ::doBoundingBoxesIntersect
*
* [1]	Module name:	  findPairs
*		Purpose:		  Search same features in two objects and return matched fatures.
* 		Last update date: 09-06-2010
*
* [2]	Module name:	  tracking
*		Purpose:		  Track the object using bounding box overlapping method. 
*		Reference:		  Joseph C.N., Kokulakumaran S., Srijeyanthan K., and Thusyanthan A., "moveIt," Final year report, 2010, pp. 41-58.
*		Algorithm:		  Bounding Box Overlapping algorithm:
*
*						  Oi(t): Object i at time=t (current frame)
*						  Oj(t-1): Object j at time = t-1 (previous frame)
*						  Bi(t): Bounding box of object i at time=t (current frame)
*						  Bj(t-1): Bounding box of object j at time=t-1 (previous frame)
*						  Vj(t-1): velocity of the object j at time=t-1 (previous frame)
*
*						  IF box overlapping (Bi(t),Bj(t-1)+Vj(t-1)) > threshold THEN
*							Oi(t) == Oj(t-1)
*						  ELSE
*							Oi(t) Oj(t-1)
*
*						  - Detect the moving blob (using background subtraction)
*						  - Label the image and draw bounding box
*						  - Get the previous frame's track
*						  - Calculate the Overlapped Area / Tracking BoundingBox Area
*						  - IF that value is greater than threshold values (0.5) THEN 
*							  both are same blob 
*						    ELSE 
*							  both are different blobs
*						  - IF both are same blobS THEN 
*						  	  update the track position based on current blob's bounding box details 
*						    ELSE 
*							  start new track
*
*
*						  ** Merging and Splitting ** 
*						  - IF blobs are merged, THEN 
*								get features of individual blob's features AND store those feature information to their	
*								respective tracks.
*						  - Track blobs while they merged.
*						  - When objects are splitted, then compare splitted blob'S features (grey scale image) against 
*						    track features
*						  - IF both feature are matched THEN both are same blob.
*		Last update date: 09-06-2010
*
* [3]	Module name:	  mvRenderTracks
*		Purpose:		  Draw bounding box over a blob and put unique blob ID. 
*		Last update date: 09-06-2010
*
* [4]	Module name:	  naiveNearestNeighbor
*		Purpose:		  Find the two nearest neighbors, but only returns the very nearest if it is distinctly better than
*						  the second nearest. 
*		Reference:		  Gary Bradski and Adrian Kaehler, Learning OpenCV,Gravenstein Highway North, Sebastopol,CA, 2008.
*		Last update date: 09-06-2010
*
* [5]	Module name:	  compareSURFDescriptors
*		Purpose:		  Compare the SURF Descriptors.
*		Reference:		  Gary Bradski and Adrian Kaehler, Learning OpenCV,Gravenstein Highway North, Sebastopol,CA, 2008.
*		Last update date: 09-06-2010
*
* [6]	Module name:	  doBoundingBoxesIntersect
*		Purpose:		  Find merged area of two boudning boxes of bolbs. This was done for previous and current frame's blobs.
*		Algorithm:		  Determining if two bounding boxes overlap:
*	
*						  //First bounding box, top left corner, bottom right corner
*						  ATLx Blob-A Top Left X-axis
*						  ATLy Blob-A Top Left Y-axis
*						  ABRx Blob-A Bottom Right X-axis
*						  ABRy Blob-A Bottom Right Y-axis
*
*						  //Second bounding box, top left corner, bottom right corner
*						  BTLx Blob-B Top Left X-axis
*						  BTLy Blob-B Top Left Y-axis
*						  BBRx Blob-B Bottom Right X-axis
*						  BBRy Blob-B Bottom Right Y-axis
*
*						  rabx = Math.abs(ATLx + ABRx - BTLx - BBRx);
*						  raby = Math.abs(ATLy + ABRy - BTLy - BBRy);
*
*						  raxPrbx = ABRx - ATLx + BBRx - BTLx;
*						  rayPrby = ATLy - ABRy + BTLy - BBRy;
*
*						  IF (rabx <= raxPrbx && raby <= rayPrby) THEN
*							blobs are overlapped
*						  ELSE
*							not overlapped
*		Last update date: 09-06-2010
*
******************************************************************************/


#pragma once

#include <sstream>
#include <highgui.h>
#include <cv.h>
#include <vector>
#include "mvTracks.h"
#include "mvBlobs.h"
#include "moveItBlob.h"

#define CV_TRACK_RENDER_ID            0x0001 ///< Print the ID of each track in the image
#define CV_TRACK_RENDER_BOUNDING_BOX  0x0002 ///< Draw bounding box of each track in the image


class mvTrack
{

public:
	~mvTrack(){}

	virtual int doBoundingBoxesIntersect(int ATLx, int ATLy, int ABRx, int ABRy, int BTLx, int BTLy, int BBRx, int BBRy)=0;
	virtual double compareSURFDescriptors( const float* d1, const float* d2, double best, int length )=0;
	virtual int naiveNearestNeighbor( const float* vec, int laplacian, const CvSeq* model_keypoints, const CvSeq* model_descriptors )=0;
	virtual void findPairs( const CvSeq* objectKeypoints, const CvSeq* objectDescriptors,const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, vector<int>& ptpairs )=0;
	virtual MvID tracking(MvBlobs const &blobs, MvTracks &tracks,  IplImage *frame, IplImage *currentframe,int thInactive , MvID maxTrackID)=0;
	virtual void renderTracks(MvTracks const tracks, IplImage *imgDest, unsigned short mode)=0;

};
