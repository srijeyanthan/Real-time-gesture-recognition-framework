/****************************************************************************
*
* mvBlob.cpp
*
* Purpose: Implementation of moveItBlob.h
*		   Common implementation for blobs.
*
* Author: Nihanth
*
*
* Namespace: moveit
* Modules:	moveit::mergeBlobs
*				  ::renderBlobs
*				  ::filterByArea
*
* Last update date: 23-06-2010
*
******************************************************************************/


#include "moveItBlob.h"

using namespace std;


namespace moveit
{

#define _MIN_(a,b) ((a)<(b)?(a):(b))
#define _MAX_(a,b) ((a)>(b)?(a):(b))

	// Filter small regions in the frame (eg: Noise)
	void filterByArea(MvBlobs &blobs, unsigned int minArea, unsigned int maxArea, IplImage *img)
	{
		CvScalar scalar;
		MvBlobs::iterator it=blobs.begin();

		while(it!=blobs.end())
		{
			MvBlob *blob=(*it).second;
			if ((blob->area<minArea)||(blob->area>maxArea))
			{

				for(int i= blob->miny; i<=blob->maxy; i++)
				{
					for(int j=blob->minx; j<=blob->maxx; j++)
					{
						scalar.val[0]=0;
						scalar.val[1]=0;
						scalar.val[2]=0;
						cvSet2D(img,i,j,scalar);
					}
				}

				delete blob;
				MvBlobs::iterator tmp=it;
				++it;
				blobs.erase(tmp);
			} else
				++it;
		}
	}


	// Merge same blobs in the frames.
	void mergeBlobs(MvBlobs &blobs)
	{
		int minx1=0, maxx1=0, xc1=0, yc1=0, xc2=0, yc2=0;

		for (MvBlobs::const_iterator it = blobs.begin(); it!=blobs.end();)
		{
			MvBlob *blob=it->second;
			minx1 = blob->minx;
			maxx1 = blob->maxx;
			xc1 = blob->centroid.x;
			yc1 = blob->centroid.y;

			for (MvBlobs::const_iterator it1 = blobs.begin(); it1!=blobs.end();)
			{
				MvBlob *blob1=it1->second;
				xc2 = blob1->centroid.x;
				yc2 = blob1->centroid.y;

				if( (minx1<=xc2 && xc2<=maxx1)  &&  (blob1!=blob)	&& (blob->area >= blob1->area) )
				{
					blob->area+=blob1->area;
					blob->minx=_MIN_(blob->minx,blob1->minx); 
					blob->maxx=_MAX_(blob->maxx,blob1->maxx);
					blob->miny=_MIN_(blob->miny,blob1->miny); 
					blob->maxy=_MAX_(blob->maxy,blob1->maxy);
					blob->m10+=blob1->m10; 
					blob->m01+=blob1->m01;
					centroid(it->second); 
					delete blob1;
					blobs.erase(it1++);
				} else 
				{
					++it1;
				}
			}
			++it;
		}
	}


	//Draw bounding box and centroid for every blobs.
	void renderBlobs(const MvBlobs &blobs, IplImage *imgDest, unsigned short mode)
	{
		if (mode)
		{
			for (MvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
			{
				MvBlob *blob=(*it).second;

				if (mode&CV_BLOB_RENDER_BOUNDING_BOX)
					cvRectangle(imgDest,cvPoint(blob->minx,blob->miny),cvPoint(blob->maxx,blob->maxy),CV_RGB(255.,0.,0.));

				if (mode&CV_BLOB_RENDER_CENTROID)
				{
					cvLine(imgDest,cvPoint(int(blob->centroid.x)-3,int(blob->centroid.y)),cvPoint(int(blob->centroid.x)+3,int(blob->centroid.y)),CV_RGB(0.,0.,255.));
					cvLine(imgDest,cvPoint(int(blob->centroid.x),int(blob->centroid.y)-3),cvPoint(int(blob->centroid.x),int(blob->centroid.y)+3),CV_RGB(0.,0.,255.));
				}
			}
		}
	}
}




