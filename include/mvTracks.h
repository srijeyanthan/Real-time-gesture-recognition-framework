/****************************************************************************
*
* mvTracks.h
*
* Purpose: Class that contain information about one Track. 
*		   - Properties of Track: 
*		   		id			
*		   		label		
*		   		minx		
*		   		maxx		
*		   		miny		
*		   		maxy		
*		   		centroid	
*		   		inactive	
*		   		merge		
*		   		action		
*		   		color		
*		   		parent		
*		   		height		
*		   		abnorm	
*				detectId
*				objectKeypoints
*				objectDescriptors
*
* Author: Nihanth
*
*
* Class: MvTrack
*
* Last update date: 23-06-2010
*
******************************************************************************/


#pragma once

#include <map>

typedef unsigned int MvLabel;
typedef unsigned int MvID;

using namespace std;


class MvTrack
{

public:
	MvID id;					// Tracked object's identification number.
	MvLabel label;				// Label assigned to the blob related to this track.
	unsigned int minx;			// minimum X pixel position of Track.
	unsigned int maxx;			// maximum X pixel position of Track.
	unsigned int miny;			// minimum Y pixel position of Track.
	unsigned int maxy;			// minimum Y pixel position of Track.
	CvPoint2D64f centroid;		// Centroid of Track.
	unsigned int inactive;		// Indicates number of frames that has been missing.
	CvSeq *objectKeypoints;		// Key points of object.
	CvSeq *objectDescriptors;	// Descriptors of object.
	bool merge;					// Check whetehr two tracks are merged or not.
	string action;				// Action of Track's object.
	CvScalar color;				// Bounding box color of Track.
	int parent;					// Parent of Track.
	unsigned int height;		// Height of Track.
	unsigned int detectId;		// Check and mark any new object apear in stage.
	bool abnorm;				// Check whether behaviour is abnormal or not.

};
