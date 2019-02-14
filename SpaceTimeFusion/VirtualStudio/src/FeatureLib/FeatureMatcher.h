#ifndef __FEATURE_MATCH_H__
#define __FEATURE_MATCH_H__

#pragma warning(disable: 4512)

#include "FeatureDefs.h"
#include "Features.h"
#include "Motion.h"

class FeatureMatcher
{	
	Motion motionConstraint;
	const FeatureSet &querySet;
	const FeatureSet &searchSet;
	uint  featureDims;

	float *matchDataForStorage;

	vector<Feature::FEATURE_TYPE> featureTypesToSearch;

public:	

	typedef struct _FeatureMatcherParams
	{
		float firstToSecondRatio;

		bool   writeToFile;
		bool   loadFromFileIfAvaliable;
		string dataFileName;
		float  motionErrThres;
		
		static _FeatureMatcherParams Default()
		{
			_FeatureMatcherParams params;
			params.firstToSecondRatio = 0.6f;
			params.writeToFile = false;
			params.loadFromFileIfAvaliable = false;
			params.motionErrThres = 100.0f;
			return params;
		}

		bool equals(const _FeatureMatcherParams compareParams)
		{
			if(firstToSecondRatio != compareParams.firstToSecondRatio) return false;

			return true;
		}

		void Dump(const string prefix)
		{
			printf("%sFeature Matcher Options:\n", prefix.c_str());
			printf("%s  firstToSecondRatio      = %f\n", prefix.c_str(), firstToSecondRatio);
			printf("%s  writeToFile             = %s\n", prefix.c_str(), ((writeToFile == true) ? "true" : "false"));
			printf("%s  loadFromFileIfAvaliable = %s\n", prefix.c_str(), ((loadFromFileIfAvaliable == true) ? "true" : "false")); 
			printf("%s  dataFileName            = %s\n", prefix.c_str(), dataFileName.c_str());

			printf("\n");
		}

	} FeatureMatcherParams;

	FeatureMatcher(const FeatureSet &queryFeatures, const FeatureSet &searchFeatures);

	void ComputeMatching(FeatureMatches &matches, Motion motionConstraint, FeatureMatcherParams params = FeatureMatcherParams::Default());

	void ComputeMatching(FeatureMatches &matches, FeatureMatcherParams params = FeatureMatcherParams::Default());

	~FeatureMatcher()
	{
		ENSURE(this->matchDataForStorage == NULL);
	}

private:
	void initMatchDataStorage();

	void writeMatchDataToFile(string fileName);

	void loadMatchesFromFile(FeatureMatches &matches, float firstToSecondRatio, string fileName);

	void exhaustiveSearch(FeatureMatches &matches, 
						  FeatureMatcherParams params,
						  Feature::FEATURE_TYPE featureType = (Feature::FEATURE_TYPE) -1);

	template <class T>
	double L2DistSqr(const vector<T> &v1, const vector<T> &v2);
};

#endif
