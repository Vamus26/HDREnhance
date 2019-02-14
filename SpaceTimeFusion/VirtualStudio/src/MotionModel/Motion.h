#ifndef __MOTION_H__
#define __MOTION_H__

#include <string>
#include "Transform-3x3.h"
#include "Definitions.h"
#include "FeatureDefs.h"
#include "Features.h"

typedef enum
{
	MT_INVALID = 0,
	MT_FUNDAMENTAL_MATRIX = 1,
	MT_HOMOGRAPHY,
	MT_RIGID_ALIGN
} MotionType;

class Motion
{
public:
    MotionType type;
	CTransform3x3 model;
	FeatureMatches matches;
	FeatureSet *sourceFeatures;
	FeatureSet *targetFeatures;

	Motion()
	{
		this->sourceFeatures = this->targetFeatures = NULL;
		this->model = CTransform3x3::Null();
		this->type = MT_INVALID;
	}

	void WriteToFile(ofstream &f);
	void WriteToFile(string fileName);
	void ReadFromFile(ifstream &f, FeatureSet *sourceFeas, FeatureSet *targetFeas);
	void ReadFromFile(string fileName, FeatureSet *sourceFeas, FeatureSet *targetFeas);

	void Dump();


	static CVector3 GetEpiLineDirc(const CVector3 &epiLine)
	{
		CVector3 epiLineDirc(-epiLine[1], epiLine[0], 0);
		return epiLineDirc.Normalize();
	}

	static CVector3 GetEpiLineNormal(const CVector3 &epiLine)
	{
		CVector3 epiLineNormal(epiLine[0], epiLine[1], 0);
		return epiLineNormal.Normalize();
	}

	static CVector3 GetEpiLine(const CTransform3x3 &fm, const Feature &featurePoint)
	{
		return GetEpiLine(fm, featurePoint.x, featurePoint.y);
	}

	static CoordFlt GetZeroCoord(CVector3 epiLine, float x, float y)
	{
		CoordFlt zeroCoord(FLT_MAX, FLT_MAX);

		///*
		CVector3 intersectLine(-epiLine[1], epiLine[0], 0);
		intersectLine[2] = -((x * intersectLine[0]) + (y * intersectLine[1]));

		if((intersectLine[1] != 0) && (epiLine[1] != 0))
		{
			intersectLine[0] /= intersectLine[1];
			intersectLine[2] /= intersectLine[1];
			intersectLine[1] /= intersectLine[1];
			epiLine[0]       /= epiLine[1];
			epiLine[2]       /= epiLine[1];
			epiLine[1]       /= epiLine[1];

			double divisor = intersectLine[0] - epiLine[0];
			ENSURE(divisor != 0);
			zeroCoord.first  = (float) ((epiLine[2] - intersectLine[2]) / divisor);
			zeroCoord.second = (float) (-(epiLine[2] + (epiLine[0] * zeroCoord.first)) / epiLine[1]);
		}
		else if(epiLine[1] == 0)
		{
			ENSURE(intersectLine[1] != 0);
			ENSURE(epiLine[0] != 0);

			zeroCoord.second = (float) -(intersectLine[2] / intersectLine[1]);
			zeroCoord.first  = (float) -(epiLine[2] / epiLine[0]);
		}
		else if(epiLine[0] == 0)
		{
			ENSURE(intersectLine[0] != 0);
			ENSURE(epiLine[1] != 0);            

			zeroCoord.first  = (float) -(intersectLine[2] / intersectLine[0]);
			zeroCoord.second = (float) -(epiLine[2] / epiLine[1]);
		}
		else
		{
			REPORT_FAILURE("Something is wrong here");
		}
		//*/

		/*
		zeroCoord.first  = x;
		zeroCoord.second = (float) (-(epiLine[2] + (epiLine[0] * zeroCoord.first)) / epiLine[1]);
		//*/
        
		return zeroCoord;
	}

	static CVector3 GetEpiLine(const CTransform3x3 &fm, double x, double y)
	{
		CVector3 point(x, y, 1.0);
		CVector3 epiLine = fm * point;
		return epiLine;
	}


	static CoordFlt GetCentroidOffset(const Motion &motion)
	{
		CoordFlt centroidOffset(0, 0);

		for(uint iMatch = 0; iMatch < motion.matches.size(); iMatch++)
		{
			FeatureMatch currMatch = motion.matches[iMatch];
			Feature &sourceFeature = motion.sourceFeatures->at(currMatch.first);
			Feature &targetFeature = motion.targetFeatures->at(currMatch.second);

			centroidOffset.first  += (targetFeature.x - sourceFeature.x);
			centroidOffset.second += (targetFeature.y - sourceFeature.y);
		}
		if(motion.matches.size() > 0)
		{
			centroidOffset.first  /= motion.matches.size();
			centroidOffset.second /= motion.matches.size();
		}

		return centroidOffset;
	}	

	static void WriteMotions(vector<Motion> &motions, string fileName);
	static void ReadMotions(vector<Motion> &motions, FeatureSet *sourceFeas, FeatureSet *targetFeas, string fileName);


	Motion GetMotionUpdate(FeatureSet *newSourceFeatures,
						   FeatureSet *newTargetFeatures, 
						   float inlierThres);
};




#endif
