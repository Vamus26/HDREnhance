#ifndef __PIECEWISE_MOTION_H__
#define __PIECEWISE_MOTION_H__

#include "FeatureDefs.h"
#include "Features.h"
#include "Motion.h"
#include "MotionFromMatches.h"

class PiecewiseMotion
{
	FeatureSet &sourceFeatures;
	FeatureSet &targetFeatures;
	FeatureMatches matches;

public:

	typedef struct _PiecewiseMotionParams
	{
		float matchesPruningMult;
		MotionFromMatches::MotionFromMatchesParams ransacParams;

		static _PiecewiseMotionParams Default()
		{
			_PiecewiseMotionParams params;
			params.ransacParams = MotionFromMatches::MotionFromMatchesParams::Default();
			params.matchesPruningMult = 1;
			return params;
		}

		void Dump(const string prefix)
		{
			printf("%sPiecewise motion params:\n", prefix.c_str());
			printf("%s  matchesPruningMult = %f\n", prefix.c_str(), matchesPruningMult);			
			string subOptPrefix = prefix + "  ";
			ransacParams.Dump(subOptPrefix);
		}		
	} PiecewiseMotionParams;


	PiecewiseMotion(FeatureSet &sourceImgFeatures,
					FeatureSet &targetImgFeatures);	

	void ComputeMotions(vector<Motion> &motions, 
						const FeatureMatches &sourceToTargetMatches,
						const PiecewiseMotionParams params);

	void ComputeMotions(vector<Motion> &motions, 
						const FeatureMatches  &sourceToTargetMatches,
						const vector<PiecewiseMotionParams> cycleParams);

private:
	void runMotionGenCycle(vector<Motion> &motions, 
						   const PiecewiseMotionParams params);

	int pruneMatches(const Motion &motion,
					 const PiecewiseMotionParams params);
};

#endif
