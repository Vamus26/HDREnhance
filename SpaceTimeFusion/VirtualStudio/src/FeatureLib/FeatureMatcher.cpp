#include "FeatureMatcher.h"
#include "Debug.h"
#include "Timer.h"
#include <math.h>
#include <fstream>
#include "MotionFromMatches.h"
#include "Disk.h"

#pragma warning(disable: 4996)


FeatureMatcher::FeatureMatcher(const FeatureSet &queryFeatures, 
							   const FeatureSet &searchFeatures) : querySet(queryFeatures), 
																   searchSet(searchFeatures)
{
	hash_set<int> queryFeaTypes;
	hash_set<int> searchFeaTypes;

	ENSURE(this->querySet.size() > 0);
	ENSURE(this->searchSet.size() > 0);

	this->featureDims = (uint) this->querySet[0].data.size();
	ENSURE(this->featureDims > 0);

	for(FeatureSet::const_iterator queryFeature = this->querySet.begin();
		queryFeature != this->querySet.end();
		queryFeature++)
	{
		ENSURE(this->featureDims == queryFeature->data.size());
		queryFeaTypes.insert((int)queryFeature->type);
	}

	for(FeatureSet::const_iterator searchFeature = this->searchSet.begin();
		searchFeature != this->searchSet.end();
		searchFeature++)
	{
		ENSURE(this->featureDims == searchFeature->data.size());
		searchFeaTypes.insert((int)searchFeature->type);
	}

	ENSURE(queryFeaTypes.size()  <= Feature::FeatureTypeCount);
	ENSURE(searchFeaTypes.size() <= Feature::FeatureTypeCount);
	this->featureTypesToSearch.clear();
	for(hash_set<int>::iterator queryType = queryFeaTypes.begin();
		queryType != queryFeaTypes.end();
		queryType++)
	{
		if(searchFeaTypes.find(*queryType) != searchFeaTypes.end())
		{
			this->featureTypesToSearch.push_back((Feature::FEATURE_TYPE) *queryType);
		}
	}
	ENSURE(this->featureTypesToSearch.size() > 0);

	this->matchDataForStorage = NULL;
}

template <class T>
double FeatureMatcher::L2DistSqr(const vector<T> &v1, const vector<T> &v2)
{
	ENSURE(v1.size() == v2.size());

	double dist = 0;
	typename vector<T>::const_iterator v1Elem;
	typename vector<T>::const_iterator v2Elem;
	for(v1Elem = v1.begin(), v2Elem = v2.begin();
		v1Elem != v1.end();
		v1Elem++, v2Elem++)
	{
		double diff = (*v1Elem) - (*v2Elem);
		dist += (diff * diff);
	}

	return dist;
}

void FeatureMatcher::exhaustiveSearch(FeatureMatches &matches, FeatureMatcherParams params, Feature::FEATURE_TYPE featureType)
{
	typedef std::pair<uint, float> InverseNeighbour;
	hash_map<uint, InverseNeighbour> inverseNeighbours;
	hash_map<uint, InverseNeighbour>::iterator findResult;

	float matchThreshold = SQR(params.firstToSecondRatio);

	int iQueryFeature = 0;
	for(FeatureSet::const_iterator queryFeature = this->querySet.begin();
		queryFeature != this->querySet.end();
		queryFeature++, iQueryFeature++)
	{
		if((featureType != -1) && (queryFeature->type != featureType))
		{
			continue;
		}

		double distBestSqr       = DBL_MAX;
		double distSecondBestSqr = DBL_MAX;
		int    indexBest         = -1;

		int iSearchFeature = 0;
		for(FeatureSet::const_iterator searchFeature = this->searchSet.begin();
			searchFeature != this->searchSet.end();
			searchFeature++, iSearchFeature++)
		{
			if((featureType != -1) && (searchFeature->type != featureType))
			{
				continue;
			}

			if(this->motionConstraint.type != MT_INVALID)
			{
				if(MotionFromMatches::IsInlier(*queryFeature, *searchFeature, 
											   this->motionConstraint, params.motionErrThres) == false)
				{
					continue;
				}
			}

			double distSqr = L2DistSqr(queryFeature->data, searchFeature->data);
			if(distSqr < distBestSqr) 
			{
				distSecondBestSqr = distBestSqr;
				distBestSqr       = distSqr;
				indexBest         = iSearchFeature;
			}
			else if(distSqr < distSecondBestSqr)
			{
				distSecondBestSqr = distSqr;
			}
		}

		if(indexBest >= 0)
		{
			float matchRatio = (float) (distBestSqr / distSecondBestSqr);

			findResult = inverseNeighbours.find(indexBest);
			if(findResult == inverseNeighbours.end())
			{
				inverseNeighbours[indexBest] = std::make_pair(iQueryFeature, matchRatio);
			}
			else
			{
				if(findResult->second.second > matchRatio)
				{
					if(matchDataForStorage != NULL)
					{
						this->matchDataForStorage[(2 * findResult->second.first) + 1] = 1.0f;
					}
					findResult->second.first  = iQueryFeature;
					findResult->second.second = matchRatio;
				}
				else
				{
					matchRatio = 1.0f;
				}
			}

			if(this->matchDataForStorage != NULL)
			{
				this->matchDataForStorage[2 * iQueryFeature]       = (float) indexBest;
				this->matchDataForStorage[(2 * iQueryFeature) + 1] = matchRatio; 
			}
		}
		else
		{
			if(this->matchDataForStorage != NULL)
			{
				this->matchDataForStorage[2 * iQueryFeature]       = 0.0f;
				this->matchDataForStorage[(2 * iQueryFeature) + 1] = 2.0f; 
			}
		}
	}


	for(hash_map<uint, InverseNeighbour>::const_iterator iMatch = inverseNeighbours.begin(); 
		iMatch != inverseNeighbours.end(); 
		iMatch++)
	{
		if(iMatch->second.second <= matchThreshold)
		{
			matches.push_back(FeatureMatch(iMatch->second.first, 
										   iMatch->first));
		}
	}
}


void FeatureMatcher::ComputeMatching(FeatureMatches &matches, FeatureMatcherParams params)	
{	
	matches.clear();

	if(params.loadFromFileIfAvaliable == true)
	{
		if(Disk::DoesFileExist(params.dataFileName) == true)
		{
			printf("Loading - %s\n", params.dataFileName.c_str());
			fflush(stdout);
			loadMatchesFromFile(matches, SQR(params.firstToSecondRatio), params.dataFileName);			
			return;
		}
	}

	printf("Computing data file - %s\n", params.dataFileName.c_str());
	fflush(stdout);

	if(params.writeToFile == true)
	{
		initMatchDataStorage();		
	}

	for(vector<Feature::FEATURE_TYPE>::const_iterator currType = this->featureTypesToSearch.begin();
		currType != this->featureTypesToSearch.end();
		currType++)
	{
		exhaustiveSearch(matches, params, *currType);
	}

	if(params.writeToFile == true)
	{
		writeMatchDataToFile(params.dataFileName);
		delete this->matchDataForStorage;
		this->matchDataForStorage = NULL;
	}
}


void FeatureMatcher::ComputeMatching(FeatureMatches &matches, Motion motionConstraint, FeatureMatcherParams params)
{
	this->motionConstraint = motionConstraint;
	ENSURE(this->motionConstraint.type != MT_INVALID);
	ComputeMatching(matches, params);
}

void FeatureMatcher::initMatchDataStorage()
{
	size_t elemCount = this->querySet.size() * 2;
	this->matchDataForStorage = new float[elemCount];
	ENSURE(this->matchDataForStorage != NULL);

	for(uint iRatio = 0; iRatio < elemCount; iRatio += 2)
	{
		this->matchDataForStorage[iRatio + 1] = 2.0f;
	}
}

void FeatureMatcher::writeMatchDataToFile(string fileName)
{
	ENSURE(this->matchDataForStorage != NULL);
	ENSURE(fileName.empty() == false);

	FILE *filePtr = fopen(fileName.c_str(), "wb");
	ENSURE(filePtr != NULL);

	size_t elemCount = this->querySet.size() * 2;

	size_t querySetSize  = this->querySet.size();
	size_t searchSetSize = this->searchSet.size();
	ENSURE(fwrite(&querySetSize,  sizeof(querySetSize),  1, filePtr) == 1);
	ENSURE(fwrite(&searchSetSize, sizeof(searchSetSize), 1, filePtr) == 1);

	size_t elemsWritten = fwrite(this->matchDataForStorage, 
								  sizeof(*this->matchDataForStorage),
								  elemCount,
								  filePtr);

	ENSURE(elemsWritten == elemCount);

	fclose(filePtr);    
}

void FeatureMatcher::loadMatchesFromFile(FeatureMatches &matches, float matioRatioThres, string fileName)
{
	ENSURE(fileName.empty() == false);

	size_t elemCount = this->querySet.size() * 2;
	float *matchData = new float[elemCount];

	FILE *filePtr = fopen(fileName.c_str(), "rb");
	ENSURE(filePtr != NULL);

	size_t querySetSize;
	size_t searchSetSize;
	ENSURE(fread(&querySetSize, sizeof(querySetSize), 1, filePtr) == 1);
	ENSURE(fread(&searchSetSize, sizeof(searchSetSize), 1, filePtr) == 1);
	ENSURE(querySetSize == this->querySet.size());
	ENSURE(searchSetSize == this->searchSet.size());

	size_t elemsRead = fread(matchData, 
							 sizeof(*matchData),
							 elemCount,
							 filePtr);
	ENSURE(elemsRead == elemCount);

	fclose(filePtr);

	int matchesFound = 0;
	for(uint iRatio = 0; iRatio < elemCount; iRatio += 2)
	{
		if(matchData[iRatio + 1] <= matioRatioThres)
		{
			matchesFound++;
		}
	}
	matches.reserve(matchesFound);

	for(uint iQueryFeature = 0; iQueryFeature < this->querySet.size(); iQueryFeature++)
	{
		float currRatio = matchData[(2 * iQueryFeature) + 1];

		if(currRatio <= matioRatioThres)
		{
			float currMatchF = matchData[(2 * iQueryFeature)];
			ENSURE(currMatchF >= 0);
			ENSURE(currMatchF == floor(currMatchF));
			uint currMatch   = (uint) currMatchF;
			ENSURE(currMatch < this->searchSet.size());

			matches.push_back(FeatureMatch(iQueryFeature, currMatch));		
		}
	}

	delete matchData;    
}
