#ifndef __PLANE_H__
#define __PLANE_H__

#include "Definitions.h"
#include "Transform-3x3.h"
#include "PointCloud.h"
#include "FeatureRenderer.h"

class Plane
{
public:
	CVector3 normal; //normal is unit normal, normal + D is eq of plane
	CVector3 point;
	double   D;
	PointCloud inlierCloud;
	CVector3 col;

	Plane();

	void Dump();

	void Write(ofstream &outStream) const;
	void Read(ifstream &outStream);

	double DistFromPt(const CVector3 &pt);

};


#endif
