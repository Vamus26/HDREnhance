#ifndef __POINT_CLOUD_H__
#define __POINT_CLOUD_H__

#include "Definitions.h"
#include "Transform-3x3.h"

typedef struct _Point
{
	CVector3 pos;
	int scenePointIndex;

	_Point(CVector3 position = CVector3(DBL_MAX, DBL_MAX, DBL_MAX),
		   int iScenePoint = -1)
	{
		this->scenePointIndex = iScenePoint;
		this->pos = position;
	}

} Point;

class PointCloud : public vector<Point>
{
public:
	void Write(ofstream &outStream) const;
	void Read(ifstream &outStream);
};


#endif
