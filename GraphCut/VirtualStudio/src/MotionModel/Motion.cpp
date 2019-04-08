#include "Motion.h"
#include "MotionFromMatches.h"
#include <fstream>

void Motion::Dump()
{
	printf("Motion Type: ");
	switch(type)
	{
	case MT_FUNDAMENTAL_MATRIX:
		printf("Fundamental Matrix");
		break;

	case MT_HOMOGRAPHY:
		printf("Homography");
		break;

	default:
		REPORT_FAILURE("Unknown motion type");
		break;
	}
	printf("\n");
	printf("Motion Inlier count: %i\n", this->matches.size());
	printf("Motion Model:\n");	
	model.Dump();
}

void Motion::WriteToFile(ofstream &f)
{
	ENSURE(f.is_open());

	f << "\n";

	f << this->model[0][0] << " " << this->model[0][1] << " " << this->model[0][2] << "\n";
	f << this->model[1][0] << " " << this->model[1][1] << " " << this->model[1][2] << "\n";
	f << this->model[2][0] << " " << this->model[2][1] << " " << this->model[2][2] << "\n";

	f << this->type << "\n";

	this->matches.Write(f);

	f << "\n";
}

void Motion::WriteToFile(string fileName)
{
	ofstream outStream(fileName.c_str());
	ENSURE(outStream.is_open());
	WriteToFile(outStream);
	outStream.close();
}


void Motion::ReadFromFile(ifstream &f, FeatureSet *sourceFeas, FeatureSet *targetFeas)
{
	ENSURE(f.is_open());

	this->sourceFeatures = sourceFeas;
	this->targetFeatures = targetFeas;

	f >> this->model[0][0];
	f >> this->model[0][1];
	f >> this->model[0][2];
	f >> this->model[1][0];
	f >> this->model[1][1];
	f >> this->model[1][2];
	f >> this->model[2][0];
	f >> this->model[2][1];
	f >> this->model[2][2];

	int intType;
	f >> intType;
	this->type = (MotionType) intType;
	ENSURE((this->type == MT_FUNDAMENTAL_MATRIX) || (this->type == MT_HOMOGRAPHY));

	this->matches.clear();
	this->matches.Read(f, sourceFeas, targetFeas);
}

void Motion::ReadFromFile(string fileName, FeatureSet *sourceFeas, FeatureSet *targetFeas)
{
	ifstream inStream(fileName.c_str());
	ENSURE(inStream.is_open());
	ReadFromFile(inStream, sourceFeas, targetFeas);	
	inStream.close();
}

void Motion::WriteMotions(vector<Motion> &motions, string fileName)
{
	ENSURE(motions.size() > 0);

	ofstream outStream(fileName.c_str());
	ENSURE(outStream.is_open());

	outStream << (int) motions.size() << "\n\n";

	for(uint iMotion = 0; iMotion < motions.size(); iMotion++)
	{
		motions[iMotion].WriteToFile(outStream);
	}

	outStream.close();
}

void Motion::ReadMotions(vector<Motion> &motions, FeatureSet *sourceFeas, FeatureSet *targetFeas, string fileName)
{
	ENSURE(motions.size() == 0);

	ifstream inStream(fileName.c_str());
	ENSURE(inStream.is_open());

	int motionCount;
	inStream >> motionCount;
	ENSURE(motionCount > 0);

	motions.resize(motionCount);
	for(uint iMotion = 0; iMotion < motions.size(); iMotion++)
	{
		motions[iMotion].ReadFromFile(inStream, sourceFeas, targetFeas);
	}

	inStream.close();
}

Motion Motion::GetMotionUpdate(FeatureSet *newSourceFeatures,
							   FeatureSet *newTargetFeatures, 
							   float inlierThres)
{
    Motion updatedMotion;

	updatedMotion.matches        = this->matches;
	updatedMotion.model          = this->model;
	updatedMotion.type           = this->type;
	updatedMotion.sourceFeatures = newSourceFeatures;
	updatedMotion.targetFeatures = newTargetFeatures;

	vector<CorrespondancePt> correspondancePts(updatedMotion.matches.size());
	for(uint iMatch = 0; iMatch < updatedMotion.matches.size(); iMatch++)
	{
		FeatureMatch matchPair = updatedMotion.matches[iMatch];
		const Feature &sourceFeature = updatedMotion.sourceFeatures->at(matchPair.first);
		const Feature &targetFeature = updatedMotion.targetFeatures->at(matchPair.second);	

		correspondancePts[iMatch].first.first   = sourceFeature.x;
		correspondancePts[iMatch].first.second  = sourceFeature.y;
		correspondancePts[iMatch].second.first  = targetFeature.x;
		correspondancePts[iMatch].second.second = targetFeature.y;
	}

	int returnVal = MotionFromMatches::SolveForMotion(correspondancePts, updatedMotion);
	ENSURE(returnVal == 0);

	for(uint iMatch = 0; iMatch < updatedMotion.matches.size(); iMatch++)
	{
		FeatureMatch matchPair = updatedMotion.matches[iMatch];
		const Feature &sourceFeature = updatedMotion.sourceFeatures->at(matchPair.first);
		const Feature &targetFeature = updatedMotion.targetFeatures->at(matchPair.second);	

		//bug
		//ENSURE(MotionFromMatches::IsInlier(sourceFeature, targetFeature, updatedMotion, inlierThres));
	}

	return updatedMotion;
}

