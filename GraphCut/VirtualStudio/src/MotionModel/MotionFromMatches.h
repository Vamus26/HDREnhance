#ifndef __MOTION_FROM_MATCHES__
#define __MOTION_FROM_MATCHES__

#pragma warning(disable: 4512)

#include "FeatureDefs.h"
#include "Features.h"
#include "Transform-3x3.h"
#include "Transform-2x2.h"
#include "Motion.h"
#include "horn.h"
#include <math.h>

class MotionFromMatches
{
	const FeatureSet     &sourceSet;
	const FeatureSet     &targetSet;
	const FeatureMatches &matches;	

	bool *matchesPicked;
	bool *targetsPicked;

public:

	const int static MinMatchesToSolveFM = 8;
	const int static MinMatchesToSolveHG = 4;
	const int static MinMatchesToSolveRA = 4;

	typedef struct _MotionFromMatchesParams
	{
		int   ransacIter;
		float inlierThres;
		int   minInliers;

		static _MotionFromMatchesParams Default()
		{
			_MotionFromMatchesParams params;
			params.ransacIter  = 5000;
			//bug - set to be  min(0.5 percentage of image size, 1)
			params.inlierThres = 4.0f;
			params.minInliers  = 16;
			return params;
		}

		void Dump(const string prefix)
		{
			printf("%sMotion From Matches Options:\n", prefix.c_str());		
			printf("%s  ransacIter  = %i\n", prefix.c_str(), ransacIter);
			printf("%s  inlierThres = %f\n", prefix.c_str(), inlierThres);
			printf("%s  minInliers  = %u\n", prefix.c_str(), minInliers);
			printf("\n");
		}
	} MotionFromMatchesParams;

	MotionFromMatches(const FeatureSet &source, 
					  const FeatureSet &target, 
					  const FeatureMatches &sourceToTargetMatches);

	~MotionFromMatches();

	Motion GetDominantMotion(FeatureMatchSet &inliers, 
						     MotionType motionType, 
							 MotionFromMatchesParams params = MotionFromMatchesParams::Default());

	static int SolveForMotion(const vector<CorrespondancePt> &correspondancePts, Motion &motion);

	static bool IsInlier(const Feature &sourceFea, const Feature &targetFea,
				 	     const Motion &motion, float inlierThres);
	
	static int GetMinMatchesToSolveMotion(MotionType type);	

	static CTransform3x3 GetFeatureTransform(const Motion &motion)
	{
		Motion feaTransformMotion;
		feaTransformMotion.type = MT_HOMOGRAPHY;

		vector<CorrespondancePt> correspondancePts(motion.matches.size());
		for(uint iMatch = 0; iMatch < motion.matches.size(); iMatch++)
		{
			FeatureMatch matchPair       = motion.matches[iMatch];
			const Feature &sourceFeature = motion.sourceFeatures->at(matchPair.first);
			const Feature &targetFeature = motion.targetFeatures->at(matchPair.second);

			correspondancePts[iMatch].first.first   = sourceFeature.x;
			correspondancePts[iMatch].first.second  = sourceFeature.y;
			correspondancePts[iMatch].second.first  = targetFeature.x;
			correspondancePts[iMatch].second.second = targetFeature.y;
		}

		int returnVal = MotionFromMatches::SolveForMotion(correspondancePts, feaTransformMotion);
		ENSURE(returnVal == 0);

		return feaTransformMotion.model;
	}


	static double GetAverageErr(const Motion &motion, CTransform3x3 homography)
	{
		double avgDist = 0;
		vector<CorrespondancePt> correspondancePts(motion.matches.size());
		for(uint iMatch = 0; iMatch < motion.matches.size(); iMatch++)
		{
			FeatureMatch matchPair       = motion.matches[iMatch];
			const Feature &sourceFeature = motion.sourceFeatures->at(matchPair.first);
			const Feature &targetFeature = motion.targetFeatures->at(matchPair.second);

			CVector3 sourcePT(sourceFeature.x, sourceFeature.y, 1);
			CVector3 targetPT(targetFeature.x, targetFeature.y, 1);

			CVector3 transPT = homography * sourcePT;
			transPT[0]       = transPT[0] / transPT[2];
			transPT[1]       = transPT[1] / transPT[2];

			double diff;
			double dist = 0;
			diff   = (transPT[0] - targetPT[0]);
			dist  += (diff * diff);
			diff   = (transPT[1] - targetPT[1]);
			dist  += (diff * diff);
			dist   = sqrt(dist);
			avgDist += dist;

			PRINT_DOUBLE(dist);
		}

		return (avgDist / motion.matches.size());
	}


private:

	void getRandomMatches(FeatureMatchSet &randSet, int count);	

	void getInliers(Motion &predictedMotion, float inlierThres, FeatureMatchSet &inliers);

	

	int solveForMotion(FeatureMatchSet &matchSet, Motion &motion);

	static int solveForFundamentalMatrix(const vector<CorrespondancePt> &pts, Motion &motion);

	static int solveForHomography(const vector<CorrespondancePt> &correspondancePts, Motion &motion);

	static int solveForRigidAlign(const vector<CorrespondancePt> &correspondancePts, Motion &motion);

	static int solveModelUsingLeastEV(float *A, int ARows, int ACols, CTransform3x3 &model);

	static bool isFundamentalMatrixInlier(const Feature &sourceFea, const Feature &targetFea,
										  const Motion &motion, float inlierThres);

	static bool isHomographyInlier(const Feature &sourceFea, const Feature &targetFea,
								   const Motion &motion, float inlierThres);	

	static bool isRigidAlignInlier(const Feature &sourceFea, const Feature &targetFea,
								   const Motion &motion, float inlierThres);

	static void IsotropicScaling(vector<CorrespondancePt> &correspondancePts,
							     CTransform3x3 &tLeft, CTransform3x3 &tRight);

	static CTransform3x3 MakeRank2(CTransform3x3 fmatrix);
};

#endif
