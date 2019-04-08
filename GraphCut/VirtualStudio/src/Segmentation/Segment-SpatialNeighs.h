#include "Definitions.h"
#ifndef __SEGMENT_SPATIAL_NEIGH_H__
#define __SEGMENT_SPATIAL_NEIGH_H__

#include "Transform-3x3.h"

//check - experiment with diag connectivity
typedef std::pair<int, float> SpatialNeighData;                 //<Boundary len, col dist sum>
typedef unordered_map<int, SpatialNeighData> SpatialNeighBookeeper;  //<neigh id, SpatialNeighData>

class SpatialNeighs
{
private:
	SpatialNeighBookeeper *neighBookeeper;

public:	
	int   *neighID;
	float *neighWeight;
	float **message;
	float **newMessage;

	int neighCount;
	int planeCount;

	SpatialNeighs();
	~SpatialNeighs();

	void FoundNeighEdgel(int neighID, float colDist);
	void DoneFindingNeighEdges();

	int  GetMessagesFileSize();
	void SerializeMessages(char **memBuffPtr);
	void UnserializeMessages(char **memBuffPtr);
};


#endif
