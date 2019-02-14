#include "Segment.h"

//check - remember to dealloc
ViewNeighs::ViewNeighs(int viewCount, int planeCount)
{
	this->segOverlapBookeeper = NULL;

	this->viewCount  = viewCount;
	this->planeCount = planeCount;
	
	this->segID           = new int**[this->viewCount];
	this->segOverlapCount = new unsigned short**[this->viewCount];
	this->segCount        = new unsigned short*[this->viewCount];
	this->projBelief      = new float*[this->viewCount];
	ENSURE(this->segID != NULL);
	ENSURE(this->segOverlapCount != NULL);
	ENSURE(this->segCount != NULL);
	ENSURE(this->projBelief != NULL);

	for(int iView = 0; iView < this->viewCount; iView++)
	{ 
		this->segID[iView]           = new int*[this->planeCount];
		this->segOverlapCount[iView] = new unsigned short*[this->planeCount];
		this->segCount[iView]        = new unsigned short[this->planeCount];
		this->projBelief[iView]      = new float[this->planeCount];
		ENSURE(this->segID[iView] != NULL);
		ENSURE(this->segOverlapCount[iView] != NULL);
		ENSURE(this->segCount[iView] != NULL);
		ENSURE(this->projBelief[iView] != NULL);

		for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
		{
			this->segID[iView][iPlane]           = NULL;
			this->segOverlapCount[iView][iPlane] = NULL;
			this->segCount[iView][iPlane]        = 0;
			this->projBelief[iView][iPlane]      = 1.0f / this->planeCount;
		}
	}
}

ViewNeighs::~ViewNeighs()
{	
	if(this->segID != NULL)
	{
		ENSURE(this->segOverlapCount != NULL);
		ENSURE(this->segCount != NULL);
		ENSURE(this->projBelief != NULL);

		for(int iView = 0; iView < this->viewCount; iView++)
		{
			ENSURE(this->segID[iView] != NULL);
			ENSURE(this->segOverlapCount[iView] != NULL);
			ENSURE(this->segCount[iView] != NULL);
			ENSURE(this->projBelief[iView] != NULL);

			for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
			{
				if(this->segID[iView][iPlane] != NULL)
					delete [] this->segID[iView][iPlane];

				if(this->segOverlapCount[iView][iPlane] != NULL)
					delete [] this->segOverlapCount[iView][iPlane];
			}

			delete [] this->segID[iView];
			delete [] this->segOverlapCount[iView];
			delete [] this->segCount[iView];
			delete [] this->projBelief[iView];
		}

		delete [] this->segID;
		delete [] this->segOverlapCount;
		delete [] this->segCount;
		delete [] this->projBelief;
	}

	if(this->segOverlapBookeeper != NULL)
		delete this->segOverlapBookeeper;
}

void ViewNeighs::StartViewPlaneBooking()
{
	ENSURE(this->segOverlapBookeeper == NULL);
	this->segOverlapBookeeper = new SegOverlapBookeeper;
}

void ViewNeighs::FoundOverlapPixel(int segID)
{
	SegOverlapBookeeper::iterator findResult = this->segOverlapBookeeper->find(segID);
	if(findResult == this->segOverlapBookeeper->end())
		this->segOverlapBookeeper->insert(std::make_pair(segID, 1));
	else
	{
		ENSURE(findResult->second != USHRT_MAX);
		findResult->second += 1;
	}
}

void ViewNeighs::DoneViewPlaneBooking(int iView, int iPlane)
{
	ENSURE(this->segOverlapBookeeper != NULL);

	ENSURE(this->segOverlapBookeeper->size() < USHRT_MAX);
	unsigned short segsFound = (unsigned short) this->segOverlapBookeeper->size();

	this->segCount[iView][iPlane]        = segsFound;
	this->segID[iView][iPlane]           = new int[segsFound];
	this->segOverlapCount[iView][iPlane] = new unsigned short[segsFound];
	ENSURE(this->segID[iView][iPlane]           != NULL);
	ENSURE(this->segOverlapCount[iView][iPlane] != NULL);

	int iOverlap = 0;
	for(SegOverlapBookeeper::const_iterator currOverlap = this->segOverlapBookeeper->begin();
		currOverlap != this->segOverlapBookeeper->end();
		currOverlap++, iOverlap++)
	{
		this->segID[iView][iPlane][iOverlap]           = currOverlap->first;
		this->segOverlapCount[iView][iPlane][iOverlap] = currOverlap->second;
	}

	delete this->segOverlapBookeeper;
	this->segOverlapBookeeper = NULL;
}


int ViewNeighs::GetStaticDataFileSize()
{
	int filesize = 0;
	for(int iView  = 0; iView  < this->viewCount;  iView++)
	for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
	{
		filesize += sizeof(unsigned short); //segCount
		int segCount = this->segCount[iView][iPlane];
		filesize += segCount * (sizeof(int) + sizeof(unsigned short)); //id + overlapCount
	}

	return filesize;
}

void ViewNeighs::SerializeStaticData(char **memBuffPtr)
{
	ENSURE(this->viewCount  > 0);
	ENSURE(this->planeCount > 0);

	char *memBuffCopy = *memBuffPtr;

	for(int iView  = 0; iView  < this->viewCount;  iView++)
	for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
	{
		unsigned short segsFound = this->segCount[iView][iPlane]; 
		WRITE_USHORT(memBuffPtr, segsFound);
		for(unsigned short iSeg = 0; iSeg < segsFound; iSeg++)
		{
			WRITE_INT(   memBuffPtr, this->segID[iView][iPlane][iSeg]);
			WRITE_USHORT(memBuffPtr, this->segOverlapCount[iView][iPlane][iSeg]);
		}
	}

	ENSURE((*memBuffPtr - memBuffCopy) == GetStaticDataFileSize());
}

void ViewNeighs::UnserializeStaticData(char **memBuffPtr)
{
	ENSURE(this->viewCount  > 0);
	ENSURE(this->planeCount > 0);

	char *memBuffCopy = *memBuffPtr;

	for(int iView  = 0; iView  < this->viewCount;  iView++)
	for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
	{
		unsigned short segsFound;
		READ_USHORT(memBuffPtr, segsFound);

		this->segCount[iView][iPlane]        = segsFound;
		this->segID[iView][iPlane]           = new int[segsFound];
		this->segOverlapCount[iView][iPlane] = new unsigned short[segsFound];
		ENSURE(this->segID[iView][iPlane] != NULL);
		ENSURE(this->segOverlapCount[iView][iPlane] != NULL);

		for(unsigned short iSeg = 0; iSeg < segsFound; iSeg++)
		{
			READ_INT(   memBuffPtr, this->segID[iView][iPlane][iSeg]);
			READ_USHORT(memBuffPtr, this->segOverlapCount[iView][iPlane][iSeg]);
		}
	}

	ENSURE((*memBuffPtr - memBuffCopy) == GetStaticDataFileSize());
}
