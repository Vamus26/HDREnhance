#include "Definitions.h"

#ifndef __SEGMENT_BOUNDS_H__
#define __SEGMENT_BOUNDS_H__

typedef struct _Bounds
{
	short minX;
	short maxX;
	short minY;
	short maxY;

	_Bounds(short minX = SHRT_MAX, short minY = SHRT_MAX, 
		    short maxX = SHRT_MIN, short maxY = SHRT_MIN)
	{
		this->minX = minX;
		this->maxX = maxX;
		this->minY = minY;
		this->maxY = maxY;
	}

	void Dump() const
	{
		printf("[(minX, minY) (maxX, maxY)] - [(%i, %i) (%i, %i)]\n", 
				this->minX, this->minY, this->maxX, this->maxY);
	}

	bool operator==(const _Bounds &b) const
	{
		return ((this->minX == b.minX) &&
				(this->minY == b.minY) &&
				(this->maxX == b.maxX) &&
				(this->maxY == b.maxY));
	}
} Bounds;

#endif
