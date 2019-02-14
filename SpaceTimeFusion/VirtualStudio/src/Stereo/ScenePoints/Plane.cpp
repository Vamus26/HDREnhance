#include "Plane.h"
#

Plane::Plane()
{
	this->col = FeatureRenderer::GetRandomColor();
}

void Plane::Dump()
{
	printf("Plane: [%lf %lf %lf %lf]\n", normal[0], normal[1], normal[2], this->D);
	PRINT_INT(inlierCloud.size());
}

void Plane::Write(ofstream &outStream) const
{
	double normLen = this->normal.Length();
	CHECK_RANGE(normLen, 0.99, 1.01);
	ENSURE(fabs(this->normal.dot(this->point) + this->D) <= 0.0001);

	outStream << "\n";
	
	this->normal.Write(outStream);
	this->point.Write(outStream);
	outStream << this->D << "\n";
	this->col.Write(outStream);
	this->inlierCloud.Write(outStream);

	outStream << "\n";
}

void Plane::Read(ifstream &inStream)
{
	this->normal.Read(inStream);
	this->point.Read(inStream);
	inStream >> this->D;	

	this->col.Read(inStream);
	this->inlierCloud.Read(inStream);

	double normLen = this->normal.Length();
	CHECK_RANGE(normLen, 0.99, 1.01);
	this->normal = this->normal.Normalize();		

	ENSURE(fabs(this->normal.dot(this->point) + this->D) <= 0.001);
}

double Plane::DistFromPt(const CVector3 &pt)
{
	return fabs(this->normal.dot(pt) + this->D);	
}