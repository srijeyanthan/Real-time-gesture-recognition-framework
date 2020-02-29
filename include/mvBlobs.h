/****************************************************************************
*
* mvBlobs.h
*
* Purpose: Class that contain information about one blob. 
*		   - Properties of blob: 
*		   		label	
*				area		
*				minx		
*				maxx		
*				miny		
*				maxy		
*				centroid	
*				m10			
*				m01			
*				_parent		
*				_rank		
*
* Author: Nihanth
*
*
* Class: MvBlob
*
* Last update date: 07-06-2010
*
******************************************************************************/


#pragma once

typedef unsigned int MvLabel;		// Define label
typedef unsigned int MvID;			// Define ID


class MvBlob
{

public:
	MvLabel label;					// Blob's label
	union
	{
		unsigned int area;			// Area of blob
	};

	unsigned int minx;				// X min
	unsigned int maxx;				// X max
	unsigned int miny;				// Y min
	unsigned int maxy;				// Y max
	CvPoint2D64f centroid;			// Centroid
	double m10;						// Moment 10
	double m01;						// Moment 01
	MvBlob *_parent;				// Parent of the union-find data estructure
	unsigned int _rank;				// Rank of the union-find data estructure

};