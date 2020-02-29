/****************************************************************************
*
* moveItBlob.h
*
* Purpose: Common base namespace for all the Blobs and Tracks.
*
* Author: Nihanth
*
*
* Namespace: moveit
* Modules:	 moveit::merge
*				   ::makeSet
*				   ::centroid
*				   ::mergeBlobs
*				   ::renderBlobs
*				   ::filterByArea
*				   ::releaseBlobs
*				   ::releaseTracks
*
* [1]	Module name:	  merge
*		Purpose:		  Merge the same blobs.
*		Reference:		  "Disjoint-set data structure", 7 july, 2010. [Online]. 
*						  Available: http://en.wikipedia.org/wiki/Disjoint-set_data_structure. 
*						  [Accessed: July 20, 2010].
* 		Last update date: 23-06-2010
*
* [2]	Module name:	  makeSet
*		Purpose:		  Make the parent and rank of the union-find data structure. 
*		Reference:		  "Disjoint-set data structure", 7 july, 2010. [Online]. 
*						  Available: http://en.wikipedia.org/wiki/Disjoint-set_data_structure. 
*						  [Accessed: July 20, 2010].
*		Last update date: 23-06-2010
*
* [3]	Module name:	  centroid
*		Purpose:		  Calculate and return the centroid of blob. 
*		Reference:		  "Centroid", January 9, 2000. [Online]. 
*						  Available: http://www.public.iastate.edu/~statics/examples/centroid/centroida.html 
*						  [Accessed: July 20, 2010].
*		Last update date: 23-06-2010
*
* [4]	Module name:	  mergeBlobs
*		Purpose:		  Merge blobs which are near to other blob. 
*		Last update date: 23-06-2010
*
* [5]	Module name:	  renderBlobs
*		Purpose:		  Draw bounding box and centroid for every blobs.
*		Last update date: 23-06-2010
*
* [6]	Module name:	  filterByArea
*		Purpose:		  Remove unwanted noise or white pixels.
*		Algorithm:		  Remove noise:
*	
*						  FOR every blobs
*							IF blobArea < minArea || blobArea > maxArea THEN
*								delete blob
*		Last update date: 23-06-2010
*
* [7]	Module name:	  releaseBlobs
*		Purpose:		  Remove every blobs from map.
*		Last update date: 23-06-2010
*
* [8]	Module name:	  releaseTracks
*		Purpose:		  Remove every tracks from map.
*		Last update date: 23-06-2010
*
******************************************************************************/


#pragma once

#include <vector>
#include <highgui.h>
#include <cv.h>

#include "mvTracks.h"
#include "mvBlobs.h"
#include "mvDhmm.h"

using namespace std;


namespace moveit
{

#define CV_BLOB_RENDER_CENTROID         0x0002		// Render centroid. \see renderBlobs
#define CV_BLOB_RENDER_BOUNDING_BOX     0x0004		// Render bounding box. \see renderBlobs
#define IPL_DEPTH_LABEL (sizeof(MvLabel)*8)

	typedef std::map<MvLabel, string> ActionList;	// map used to store actions
	typedef std::pair<int, string>  ActionListPair;	// pair of ActionList
	typedef std::map<MvLabel,MvBlob *> MvBlobs;		// map used to store detected white object
	typedef std::pair<MvLabel,MvBlob *> MvLabelBlob;// pair of MvBlobs
	typedef std::map<MvID, MvTrack *> MvTracks;		// map used to store track
	typedef std::pair<MvID, MvTrack *> MvIDTrack;	// pair of MvTracks

	void makeSet(MvBlob *blob);
	void merge(MvBlob *blob1, MvBlob *blob2);
	void mergeBlobs(MvBlobs &blobs);

	inline void releaseBlobs(MvBlobs &blobs)
	{
		for (MvBlobs::iterator it=blobs.begin(); it!=blobs.end(); it++)
		{
			MvBlob *blob = (*it).second;
			if (blob) delete blob;
		}

		blobs.clear();
	}

	inline CvPoint2D64f centroid(MvBlob *blob)
	{
		return blob->centroid=cvPoint2D64f(blob->m10/blob->area, blob->m01/blob->area);
	}

	inline void releaseTracks(MvTracks &tracks)
	{
		for (MvTracks::iterator it=tracks.begin(); it!=tracks.end(); it++)
		{
			MvTrack *track = (*it).second;
			if (track) delete track;
		}

		tracks.clear();
	}

	void renderBlobs(const MvBlobs &blobs, IplImage *imgDest, unsigned short mode=0x000f);
	void filterByArea(MvBlobs &blobs, unsigned int minArea, unsigned int maxArea,IplImage *);
};