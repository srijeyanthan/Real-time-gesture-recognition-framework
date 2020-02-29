/****************************************************************************
*
* mvBuffer.h
*
* Purpose: Cyclic buffer that holds set of bufferObjects, each bufferObjects holds 10 frames.
*
* Author: Nihanth
*
*
* Class: Buffer
* Modules:	Buffer::end
*				  ::begin
*				  ::setEnd
*				  ::maxSize
*
* 
* [1]	Module name:	  end
*		Purpose:		  Pointer to one past frame in the bufferObject.
*		Last update date: 07-06-2010
* 
* [2]	Module name:	  begin
*		Purpose:		  Pointer to first frame in the bufferObject.
*		Last update date: 07-06-2010
* 
* [3]	Module name:	  setEnd
*		Purpose:		  Set end of buffer.
*		Last update date: 07-06-2010
*
* [4]	Module name:	  maxSize
*		Purpose:		  Number of frames a buffer can hold.
*		Last update date: 07-06-2010
*
******************************************************************************/


#pragma once

#include "mvThread.h"


class Buffer
{

private:
	static const size_t bufferSize = 10;		// Buffer Size - Number of buffer objects a buffer has.
    BufferObject* mvEnd;					
    BufferObject storage[bufferSize];			// Cyclic buffer.


public:
    Buffer(void);
	~Buffer(void);

    BufferObject* begin() 
	{
		return storage;
	}
    const BufferObject* begin() const 
	{
		return storage;
	}   

    BufferObject* end() const 
	{
		return mvEnd;
	}   

    void setEnd( BufferObject* ptr ) 
	{
		mvEnd=ptr;
	}   

    size_t maxSize() const 
	{
		return bufferSize;
	}  

};
