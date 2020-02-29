/****************************************************************************
*
* mvBgsParams.h
*
* Purpose: Base class for BGS parameters. Any parameters common to all BGS algorithms should be specified directly in this class.
*
* Author: Thusy, Nihanth, Sri
*
*
* Namespace: BackgroundSubtraction
* Class: BgsParams
* Modules:	BackgroundSubtraction::BgsParams::setFrameSize
*											::mvWidth
*											::mvHeight
*											::mvSize
*
* [1]	Module name:	  setFrameSize
* 		Purpose:		  Set image parameters.
*		Last update date: 01-06-2010
* 
* [2]	Module name:	  mvWidth
*		Purpose:		  Set and get width.
*		Last update date: 01-06-2010
*
* [3]	Module name:	  mvHeight
*		Purpose:		  Set and get height.
*		Last update date: 01-06-2010
*
* [4]	Module name:	  mvSize
*		Purpose:		  Set and get size.
*		Last update date: 01-06-2010
*
******************************************************************************/


#pragma once


namespace BackgroundSubtraction
{

	class BgsParams
	{

	protected:
		unsigned int width;		// Width of frame
		unsigned int height;	// Height of frame
		unsigned int size;		// Frame size


	public:
		virtual ~BgsParams() {}

		virtual void setFrameSize(unsigned int w, unsigned int h)
		{
			width = w;
			height = h;
			size = width*height;
		}

		unsigned int &mvWidth() 
		{ 
			return width; 
		}

		unsigned int &mvHeight() 
		{ 
			return height; 
		}

		unsigned int &mvSize() 
		{ 
			return size; 
		}

	};

};


