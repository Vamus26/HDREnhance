#include "Segment.h"

int Segment::GetStaticDataFileSize()
{
	int fileSize = 1 * sizeof(int)   + //id
				   1 * sizeof(int)   + //size	
		           4 * sizeof(short) + //bounds
				   2 * sizeof(float) + //point
				   3 * sizeof(float);  //avgCol

	ENSURE(this->segData    != NULL);
	ENSURE(this->viewNeighs != NULL);
	fileSize += this->segData->GetStaticDataFileSize();
	fileSize += this->viewNeighs->GetStaticDataFileSize();

	return fileSize;
}

void Segment::SerializeStaticData(char **memBuffPtr)
{
	WRITE_INT(memBuffPtr, this->id);

	ENSURE(this->size < USHRT_MAX);
	WRITE_INT(memBuffPtr, this->size);

	WRITE_SHORT(memBuffPtr, this->bounds.minX);
	WRITE_SHORT(memBuffPtr, this->bounds.minY);
	WRITE_SHORT(memBuffPtr, this->bounds.maxX);
	WRITE_SHORT(memBuffPtr, this->bounds.maxY);

	WRITE_FLOAT(memBuffPtr, this->point[0]);
	WRITE_FLOAT(memBuffPtr, this->point[1]);

	WRITE_FLOAT(memBuffPtr, this->avgCol[0]);
	WRITE_FLOAT(memBuffPtr, this->avgCol[1]);
	WRITE_FLOAT(memBuffPtr, this->avgCol[2]);

	this->segData->SerializeStaticData(memBuffPtr);
	this->viewNeighs->SerializeStaticData(memBuffPtr);
}

void Segment::UnserializeStaticData(char **memBuffPtr)
{
	READ_INT(memBuffPtr, this->id);

	READ_INT(memBuffPtr, this->size);
	ENSURE(this->size < USHRT_MAX);

	READ_SHORT(memBuffPtr, this->bounds.minX);
	READ_SHORT(memBuffPtr, this->bounds.minY);
	READ_SHORT(memBuffPtr, this->bounds.maxX);
	READ_SHORT(memBuffPtr, this->bounds.maxY);

	READ_FLOAT(memBuffPtr, this->point[0]);
	READ_FLOAT(memBuffPtr, this->point[1]);
	
	READ_FLOAT(memBuffPtr, this->avgCol[0]);
	READ_FLOAT(memBuffPtr, this->avgCol[1]);
	READ_FLOAT(memBuffPtr, this->avgCol[2]);

	this->segData->UnserializeStaticData(memBuffPtr);
	this->viewNeighs->UnserializeStaticData(memBuffPtr);
}

