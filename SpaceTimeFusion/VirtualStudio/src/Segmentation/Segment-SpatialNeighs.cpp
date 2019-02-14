#include "Segment.h"

SpatialNeighs::SpatialNeighs()
{
	this->neighBookeeper = NULL;

	this->neighID      = NULL;	
	this->neighWeight  = NULL;
	this->message      = NULL;
	this->newMessage   = NULL;

	this->neighCount = 0;
	this->planeCount = 0;
}

SpatialNeighs::~SpatialNeighs()
{
	if(this->neighBookeeper != NULL)
		delete this->neighBookeeper;

	if(this->neighID != NULL)
		delete [] this->neighID;

	if(this->neighWeight != NULL)
		delete [] this->neighWeight;

	if(this->newMessage != NULL)
	{
		ENSURE(this->message != NULL);
		for(int iNeigh = 0; iNeigh < this->neighCount; iNeigh++)
		{
			delete [] this->message[iNeigh];
			delete [] this->newMessage[iNeigh];			
		}
		delete [] this->message;
		delete [] this->newMessage;
	}
}

void SpatialNeighs::FoundNeighEdgel(int neighID, float colDist)
{
	if(this->neighBookeeper == NULL)
	{
		this->neighBookeeper = new SpatialNeighBookeeper;
		ENSURE(this->neighBookeeper != NULL);
	}

	SpatialNeighBookeeper::iterator findResult = this->neighBookeeper->find(neighID);
	if(findResult == this->neighBookeeper->end())
	{
		SpatialNeighData initNeighData(1, colDist); 
		this->neighBookeeper->insert(std::make_pair(neighID, initNeighData));
	}
	else
	{
		findResult->second.first  += 1;
		findResult->second.second += colDist;
	}
}

void SpatialNeighs::DoneFindingNeighEdges()
{
	ENSURE(this->neighBookeeper != NULL);
		
	this->neighCount = (int) this->neighBookeeper->size();
	ENSURE(this->neighCount > 0);
	
	this->neighID = new int[this->neighCount];
	ENSURE(this->neighID != NULL);

	this->neighWeight = new float[this->neighCount];
	ENSURE(this->neighWeight != NULL);

	int boundaryLen = 0;
	for(SpatialNeighBookeeper::const_iterator currNeigh = this->neighBookeeper->begin();
		currNeigh != this->neighBookeeper->end();
		currNeigh++)
	{
		boundaryLen += currNeigh->second.first;
	}

	int iNeigh = 0;
	for(SpatialNeighBookeeper::const_iterator currNeigh = this->neighBookeeper->begin();
		currNeigh != this->neighBookeeper->end();
		currNeigh++, iNeigh++)
	{
		this->neighID[iNeigh] = currNeigh->first;		
		float avgColDist  = currNeigh->second.second / currNeigh->second.first; 
		float expArg      = avgColDist * avgColDist / (-0.007f * 3); //check - turn into params
		//float expArg      = avgColDist * avgColDist / (-0.024f * 3); //check - turn into params
		
		float currNeighW  = exp(expArg) * ((float) currNeigh->second.first / boundaryLen);		
		currNeighW = pow(currNeighW, 0.5f);
		
		this->neighWeight[iNeigh] = (0.8f * currNeighW) + 0.001f;
		//this->neighWeight[iNeigh] = (0.8f * currNeighW) + 0.2f;
	}

	ENSURE(this->neighCount != 0);
	this->message    = new float*[this->neighCount];
	this->newMessage = new float*[this->neighCount];
	ENSURE(this->message    != NULL);
	ENSURE(this->newMessage != NULL);
	ENSURE(this->planeCount != 0);	
	for(int iNeigh = 0; iNeigh < this->neighCount; iNeigh++)
	{
		this->message[iNeigh]    = new float[this->planeCount];
		this->newMessage[iNeigh] = new float[this->planeCount];
		ENSURE(this->message[iNeigh]    != NULL);
		ENSURE(this->newMessage[iNeigh] != NULL);
		for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
		{
			this->message[iNeigh][iPlane] = this->newMessage[iNeigh][iPlane] = 1.0f / this->planeCount;
		}
	}

	delete this->neighBookeeper;
	this->neighBookeeper = NULL;
}


int SpatialNeighs::GetMessagesFileSize()
{
	//neighCount + messages
	return sizeof(int) +
		   (this->neighCount * this->planeCount * sizeof(float));
}

void SpatialNeighs::SerializeMessages(char **memBuffPtr)
{
	ENSURE(this->neighCount > 0);
	ENSURE(this->planeCount > 0);

	char *memBuffCopy = *memBuffPtr;

	WRITE_INT(memBuffPtr, this->neighCount);
	for(int iNeigh = 0; iNeigh < this->neighCount; iNeigh++)
	{
		float msgSum = 0.0f;
		for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
		{
			WRITE_FLOAT(memBuffPtr, this->message[iNeigh][iPlane]);
			msgSum +=  this->message[iNeigh][iPlane];
		}
		CHECK_RANGE(fabs(msgSum - 1.0f), 0.0f, 0.01f);
	}
	
	ENSURE((*memBuffPtr - memBuffCopy) == GetMessagesFileSize());
}

void SpatialNeighs::UnserializeMessages(char **memBuffPtr)
{
	ENSURE(this->neighCount > 0);
	ENSURE(this->planeCount > 0);

	char *memBuffCopy = *memBuffPtr;

	int nCount = 0;
	READ_INT(memBuffPtr, nCount);
	ENSURE(nCount = this->neighCount);	
	for(int iNeigh = 0; iNeigh < this->neighCount; iNeigh++)
	{
		float msgSum = 0.0f;
		for(int iPlane = 0; iPlane < this->planeCount; iPlane++)
		{
			READ_FLOAT(memBuffPtr, this->message[iNeigh][iPlane]);
			msgSum +=  this->message[iNeigh][iPlane];
		}
		CHECK_RANGE(fabs(msgSum - 1.0f), 0.0f, 0.01f);
	}

	ENSURE((*memBuffPtr - memBuffCopy) == GetMessagesFileSize());
}


