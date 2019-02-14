#include "PiecewiseMotion.h"
#include "Timer.h"

PiecewiseMotion::PiecewiseMotion(FeatureSet &sourceImgFeatures,
								 FeatureSet &targetImgFeatures) : sourceFeatures(sourceImgFeatures),
																  targetFeatures(targetImgFeatures)
{
}

int PiecewiseMotion::pruneMatches(const Motion &motion,
								  const PiecewiseMotionParams params)
{
	ENSURE(params.matchesPruningMult >= 1.0f);

	int matchesPruned = 0;
	float inlierThres = params.ransacParams.inlierThres * params.matchesPruningMult;
	for(uint iMatch = 0; iMatch < this->matches.size(); iMatch++)
	{
		FeatureMatch currMatch = this->matches[iMatch];
		Feature &sourceFeature = this->sourceFeatures[currMatch.first];
		Feature &targetFeature = this->targetFeatures[currMatch.second];

		if(MotionFromMatches::IsInlier(sourceFeature, targetFeature, 
									   motion, inlierThres) == true)
		{
			FeatureMatches::iterator matchToDel = this->matches.begin() + iMatch;
			this->matches.erase(matchToDel, matchToDel + 1);
			iMatch--;
			matchesPruned++;
		}
	}
	
	ENSURE(matchesPruned > 0);
	return matchesPruned;
}



void PiecewiseMotion::runMotionGenCycle(vector<Motion> &motions, 
										PiecewiseMotionParams params)
{
	//bug - gen multiple motion types
	MotionType motionType = MT_FUNDAMENTAL_MATRIX;
	//MotionType motionType = MT_HOMOGRAPHY;
	int minMatchesToSolveMotion = MotionFromMatches::GetMinMatchesToSolveMotion(motionType);

	while((int) this->matches.size() >= minMatchesToSolveMotion)
	{
		Motion dominantMotion;

		FeatureMatchSet motionInliers;
		MotionFromMatches motionMachine(this->sourceFeatures, this->targetFeatures, this->matches);
		dominantMotion = motionMachine.GetDominantMotion(motionInliers, motionType, params.ransacParams);

		printf("\t[%i, %i]\n", this->matches.size(), motionInliers.size());

		if((int) motionInliers.size() < minMatchesToSolveMotion)
		{
			break;
		}

		pruneMatches(dominantMotion, params);

        motions.push_back(dominantMotion);
	}
}

void PiecewiseMotion::ComputeMotions(vector<Motion> &motions, 
									 const FeatureMatches  &sourceToTargetMatches,
									 vector<PiecewiseMotionParams> cycleParams)
{
    this->matches = sourceToTargetMatches;

	for(uint iRun = 0; iRun < cycleParams.size(); iRun++)
	{
		runMotionGenCycle(motions, cycleParams[iRun]);
	}
}

void PiecewiseMotion::ComputeMotions(vector<Motion> &motions, 
									 const FeatureMatches  &sourceToTargetMatches,
									 PiecewiseMotionParams params)
{
	vector<PiecewiseMotionParams> cycleParams;
	cycleParams.push_back(params);

	printf("\n[Matchsize, inlier-count]\n");
	ComputeMotions(motions, sourceToTargetMatches, cycleParams);
}
