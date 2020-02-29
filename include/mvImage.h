/****************************************************************************
*
* mvImage.h
*
* Purpose:  C++ wrapper for OpenCV IplImage which supports simple and efficient access to the image data
*
* Author: Thusy, Nihanth, Sri
*
* Class: BwImage
* Class: RgbPixel
* Class: RgbImage
* Class: ImageBase
*
******************************************************************************/


#pragma once

#include <cv.h>
#include <cxcore.h>

const unsigned char NUM_CHANNELS = 3;


class RgbPixel
{

public:
	RgbPixel() {;}
	RgbPixel(unsigned char _r, unsigned char _g, unsigned char _b)
	{
		ch[0] = _r; ch[1] = _g; ch[2] = _b;
	}

	RgbPixel& operator=(const RgbPixel& rhs)
	{
		ch[0] = rhs.ch[0]; ch[1] = rhs.ch[1]; ch[2] = rhs.ch[2];
		return *this;
	}

	inline unsigned char& operator()(const int _ch)
	{
		return ch[_ch];
	}

	inline unsigned char operator()(const int _ch) const
	{
		return ch[_ch];
	}

	unsigned char ch[3];
};



class ImageBase
{

public:
 	ImageBase(IplImage* img = NULL) { imgp = img; m_bReleaseMemory = true; 
	}
	~ImageBase();

	void ReleaseMemory(bool b) { m_bReleaseMemory = b; }

	IplImage* Ptr() { return imgp; }
	const IplImage* Ptr() const { return imgp; }

	void operator=(IplImage* img) 
	{ 
		imgp = img;
	}

	ImageBase(const ImageBase& rhs)
	{	
		assert(false);
	}

	ImageBase& operator=(const ImageBase& rhs)
	{
		assert(false);

		return *this;
	}

	virtual void Clear() = 0;


protected:
	IplImage* imgp;
	bool m_bReleaseMemory;
};



class RgbImage : public ImageBase
{
public:
	RgbImage(IplImage* img = NULL) : ImageBase(img) { ; }

	virtual void Clear()
	{
		cvZero(imgp);
	}

	void operator=(IplImage* img) 
	{ 
		imgp = img;
	}

	// channel-level access using image(row, col, channel)
	inline unsigned char& operator()(const int r, const int c, const int ch)
	{
		return (unsigned char &)imgp->imageData[r*imgp->widthStep+c*imgp->nChannels+ch];
	}

	inline const unsigned char& operator()(const int r, const int c, const int ch) const
	{
		return (unsigned char &)imgp->imageData[r*imgp->widthStep+c*imgp->nChannels+ch];
	}

	// RGB pixel-level access using image(row, col)
	inline RgbPixel& operator()(const int r, const int c) 
	{
		return (RgbPixel &)imgp->imageData[r*imgp->widthStep+c*imgp->nChannels];
	}

	inline const RgbPixel& operator()(const int r, const int c) const
	{
		return (RgbPixel &)imgp->imageData[r*imgp->widthStep+c*imgp->nChannels];
	}
};



class BwImage : public ImageBase
{

public:
	BwImage(IplImage* img = NULL) : ImageBase(img) { ; }

	virtual void Clear()
	{
		cvZero(imgp);
	}

	void operator=(IplImage* img) 
	{ 
		imgp = img;
	}

	// pixel-level access using image(row, col)
	inline unsigned char& operator()(const int r, const int c)
	{
		return (unsigned char &)imgp->imageData[r*imgp->widthStep+c];
	}

	inline unsigned char operator()(const int r, const int c) const
	{
		return (unsigned char)imgp->imageData[r*imgp->widthStep+c];
	}


};

