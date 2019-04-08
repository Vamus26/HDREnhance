#include "PointCloud.h"

void PointCloud::Write(ofstream &outStream) const
{
	outStream << "\n";
	outStream << this->size() << "\n";
	for(vector<Point>::const_iterator currPoint = this->begin(); 
		currPoint != this->end();
		currPoint++)
	{
		currPoint->pos.Write(outStream);
		outStream << currPoint->scenePointIndex << "\n";
	}
	outStream << "\n";
}

void PointCloud::Read(ifstream &inStream)
{
	ENSURE(this->size() == 0);

	int intType;

	inStream >> intType;
	ENSURE(intType >= 0);
	this->resize(intType);

	for(uint iPoint = 0; iPoint < this->size(); iPoint++)
	{
		this->at(iPoint).pos.Read(inStream);

		intType = -1;
		inStream >> intType;
		ENSURE(intType >= 0);
		this->at(iPoint).scenePointIndex = intType;
	}
}
