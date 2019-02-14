#ifndef __FEATURE_DEFS_H__
#define __FEATURE_DEFS_H__

#include "Definitions.h"
#include <fstream>

typedef std::pair<uint, uint> FeatureMatch;
typedef vector<uint>          FeatureMatchSet;
typedef std::pair<float, float> CoordFlt;
typedef std::pair<CoordFlt, CoordFlt> CorrespondancePt;

typedef struct _Correspondances
{
	int queryIndex;
	int searchIndex;
	int plane;
	vector<CorrespondancePt> correspondancePts;


	_Correspondances()
	{
		this->queryIndex = this->searchIndex = this->plane = -1;
	}

} Correspondances;



#endif
