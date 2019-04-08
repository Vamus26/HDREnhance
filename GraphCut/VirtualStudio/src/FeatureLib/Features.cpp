#include <fstream>
#include <math.h>
#include "Definitions.h"
#include "Features.h"
#include "Debug.h"
#include <algorithm>
#include "Disk.h"

Feature::FEATURE_TYPE Feature::FeatureTypeList[];
char *Feature::FeatureTypeFileExt[];
Feature::_InitFeatureTypeConstants Feature::_InitFeatureTypeConstants::init;

Feature::Feature() {
	this->x      = -1.0f;
	this->y      = -1.0f;
	this->type   = (Feature::FEATURE_TYPE) -1;
}

bool Feature::read_Mikolajczyk(istream &is, CShape imgShape, FEATURE_TYPE featuretype, float scale,
							   const CByteImage &pruneMask, bool useMask)
{
	this->type = featuretype;

	is >> this->x;
	is >> this->y;
	this->x *= scale;
	this->y *= scale;
	this->y = (imgShape.height - 1) - this->y;

	if((this->x < 0) && (this->x >= -1)) x = 0;
	if((this->y < 0) && (this->y >= -1)) y = 0;
	if((this->x <= imgShape.width)  && (this->x > imgShape.width  - 1.0f)) this->x = imgShape.width  - 1.0f;
	if((this->y <= imgShape.height) && (this->y > imgShape.height - 1.0f)) this->y = imgShape.height - 1.0f;
		
	ENSURE(imgShape.InBounds(this->x, this->y));

	float dummy;
	is >> dummy;
	is >> dummy;
	is >> dummy;

	data.resize(128);
	for (int i = 0; i < 128; i++) 
	{
		int feaVecElem;
		is >> feaVecElem;
		ENSURE(feaVecElem <= Feature::DataMax);
		ENSURE(feaVecElem >= Feature::DataMin);
		data[i] = (uchar) feaVecElem;
	}

	if(useMask == true)
	{
		ENSURE(pruneMask.Shape().SameIgnoringNBands(imgShape));
		ENSURE(pruneMask.Shape().nBands == 1);
		int xx = NEAREST_INT(this->x);
		int yy = NEAREST_INT(this->y);
		if(pruneMask.Pixel(xx, yy, 0) == MASK_INVALID)
		{
			return false;
		}
	}


	return true;
}

bool Feature::read_sift(istream &is, CShape imgShape, float scale, 
						const CByteImage &pruneMask, bool useMask) 
{
	
	this->type = SIFT_FEATURES;

	is >> this->y;
	is >> this->x;
	this->x *= scale;
	this->y *= scale;
	this->y = (imgShape.height - 1) - this->y;	

	if((this->x < 0) && (this->x >= -1)) x = 0;
	if((this->y < 0) && (this->y >= -1)) y = 0;
	if((this->x <= imgShape.width)  && (this->x > imgShape.width  - 1.0f)) this->x = imgShape.width  - 1.0f;
	if((this->y <= imgShape.height) && (this->y > imgShape.height - 1.0f)) this->y = imgShape.height - 1.0f;

	ENSURE(imgShape.InBounds(this->x, this->y));

	float dummy;	
	is >> dummy;
	is >> dummy;

	data.resize(128);
	for (int i = 0; i < 128; i++) {
		int feaVecElem;
		is >> feaVecElem;
		ENSURE(feaVecElem <= Feature::DataMax);
		ENSURE(feaVecElem >= Feature::DataMin);
		data[i] = (uchar) feaVecElem;
	}

	if(useMask == true)
	{
		ENSURE(pruneMask.Shape().SameIgnoringNBands(imgShape));
		ENSURE(pruneMask.Shape().nBands == 1);
		int xx = NEAREST_INT(this->x);
		int yy = NEAREST_INT(this->y);
		if(pruneMask.Pixel(xx, yy, 0) == MASK_INVALID)
		{
			return false;
		}
	}

	return true;
}


void Feature::write_sift(ostream &os, CShape imgShape) 
{
	ENSURE(imgShape.InBounds(this->x, this->y));
	float siftY = (imgShape.height - 1) - this->y;

	os << siftY << " " << this->x << " 1.0 0.0";

	for(uint iData = 0; iData < this->data.size(); iData++)
	{
		if((iData % 20) == 0) 
			os << "\n";
		os << ((int) this->data[iData]) << " ";
	}
	os << "\n";
}


bool Feature::readCustom(istream &is, CShape imgShape, float scale,
					     const CByteImage &pruneMask, bool useMask)
{
	int typeInt;
	is >> typeInt;	
	this->type = (FEATURE_TYPE) typeInt;
	ENSURE(this->type >= 0);
	ENSURE(this->type < this->FeatureTypeCount);

	is >> this->x;
	is >> this->y;
	this->x *= scale;
	this->y *= scale;
	//this->y = (imgShape.height - 1) - this->y;	

	if((this->x < 0) && (this->x >= -1)) x = 0;
	if((this->y < 0) && (this->y >= -1)) y = 0;
	if((this->x <= imgShape.width)  && (this->x > imgShape.width  - 1.0f)) this->x = imgShape.width  - 1.0f;
	if((this->y <= imgShape.height) && (this->y > imgShape.height - 1.0f)) this->y = imgShape.height - 1.0f;

	ENSURE(imgShape.InBounds(this->x, this->y));

	data.resize(128);
	for (int i = 0; i < 128; i++) {
		int feaVecElem;
		is >> feaVecElem;
		ENSURE(feaVecElem <= Feature::DataMax);
		ENSURE(feaVecElem >= Feature::DataMin);
		data[i] = (uchar) feaVecElem;
	}

	if(useMask == true)
	{
		ENSURE(pruneMask.Shape().SameIgnoringNBands(imgShape));
		ENSURE(pruneMask.Shape().nBands == 1);
		int xx = NEAREST_INT(this->x);
		int yy = NEAREST_INT(this->y);
		if(pruneMask.Pixel(xx, yy, 0) == MASK_INVALID)
		{
			return false;
		}
	}

	return true;
}

void Feature::writeCustom(ofstream &f)
{
	f << (int) this->type << " " <<	(float) this->x << " " << (float) this->y;

	for(int iData = 0; iData < (int) this->data.size(); iData++)
	{
		f << " " << ((int) this->data[iData]);
	}

	f << "\n";
}


bool FeatureSet::Load(const char *name, CShape imgShape, Feature::FEATURE_TYPE featuretype, float scale,
					  const CByteImage &pruneMask, bool useMask)
{
	switch(featuretype)
	{
	case Feature::SIFT_FEATURES:
		return load_sift(name, imgShape, scale, 
						 pruneMask, useMask);
		break;

	case Feature::HAR_FEATURES:
	case Feature::HES_FEATURES:
	case Feature::MSER_FEATURES:
		return load_Mikolajczyk(name, imgShape, featuretype, scale, 
								pruneMask, useMask);
		break;

	default:
		REPORT_FAILURE("Unknown feature type - %i\n", featuretype);
		break;
	}

	return false;
}

bool FeatureSet::load_sift(const char *name, CShape imgShape, float scale,
						   const CByteImage &pruneMask, bool useMask)
{
	ifstream f(name);
	ENSURE(f.is_open());

	int featureCount, featureDims;
	f >> featureCount;
	f >> featureDims;
	ENSURE(featureDims == 128);

	size_t oldSize = size();
	resize(oldSize + featureCount);
	iterator currFeature = begin() + oldSize;
	int featuresParsed = 0;
	while (featuresParsed != featureCount) 
	{		
		if(currFeature->read_sift(f, imgShape, scale, 
			  				      pruneMask, useMask) == true)
		{
			currFeature++;
		}
		featuresParsed++;
	}
	erase(currFeature, end());

	f.close();
	return true;
}

bool FeatureSet::load_Mikolajczyk(const char *name, CShape imgShape, Feature::FEATURE_TYPE featuretype, float scale,
								  const CByteImage &pruneMask, bool useMask)
{
	ifstream f(name);
	ENSURE(f.is_open());

	int featureCount, featureDims;
	f >> featureDims;
	f >> featureCount;	
	ENSURE(featureDims == 128);

	size_t oldSize = size();
	resize(oldSize + featureCount);
	iterator currFeature = begin() + oldSize;
	int featuresParsed = 0;
	while (featuresParsed != featureCount)
	{		
		if(currFeature->read_Mikolajczyk(f, imgShape, featuretype, scale, 
									     pruneMask, useMask) == true)
		{
			currFeature++;
		}
		featuresParsed++;
	}

	erase(currFeature, end());

	f.close();
	return true;
}

bool FeatureSet::SaveCustom(const char *name)
{
	ofstream f(name);

	ENSURE(this->size() > 0)
	f << this->at(0).data.size() << " "  << this->size() << "\n";

	for(FeatureSet::iterator currFeature = this->begin(); 
		currFeature != this->end();
		currFeature++)			
	{
		currFeature->writeCustom(f);
	}

	f.close();
	return true;
}

bool FeatureSet::SaveSift(const char *name, CShape imgShape)
{
	ofstream f(name);

	ENSURE(this->size() > 0)
	f << this->size() << " " << this->at(0).data.size() << "\n";

	for(FeatureSet::iterator currFeature = this->begin(); 
		currFeature != this->end();
		currFeature++)			
	{
		currFeature->write_sift(f, imgShape);
	}

	f.close();
	return true;
}

bool FeatureSet::LoadCustom(const char *name, CShape imgShape, float scale,
								  const CByteImage &pruneMask, bool useMask)
{
	ifstream f(name);
	ENSURE(f.is_open());

	int featureCount, featureDims;
	f >> featureDims;
	f >> featureCount;	
	ENSURE(featureDims == 128);

	size_t oldSize = size();
	resize(oldSize + featureCount);
	iterator currFeature = begin() + oldSize;
	int featuresParsed = 0;
	while (featuresParsed != featureCount)
	{		
		if(currFeature->readCustom(f, imgShape, scale, 
								   pruneMask, useMask) == true)
		{
			currFeature++;
		}
		featuresParsed++;
	}

	f.close();
	return true;
}

bool FeatureSet::LoadAll(string baseName, CShape imgShape, float scale,
						 const CByteImage &pruneMask, bool useMask)
{
	bool atleastOneFileLoaded = false;

	//string feaFileName = baseName + FEA_EXT;
	//if(Disk::DoesFileExist(feaFileName))
	//{
	//	printf("Loading - %s\n", feaFileName.c_str());
	//	bool loadSuccess = LoadCustom(feaFileName.c_str(), imgShape, scale, pruneMask, useMask);
	//	ENSURE(loadSuccess == true);
	//	atleastOneFileLoaded = true;
	//}
	//else
	{
		for(int iType = 0; iType < Feature::FeatureTypeCount; iType++)	
		{
			string fileName = baseName + Feature::FeatureTypeFileExt[iType];
			if(Disk::DoesFileExist(fileName))
			{
				printf("Loading - %s\n", fileName.c_str());
				bool loadSuccess = Load(fileName.c_str(), imgShape, Feature::FeatureTypeList[iType], scale,
										pruneMask, useMask);
				ENSURE(loadSuccess == true);
				atleastOneFileLoaded = true;
			}
		}

		if(atleastOneFileLoaded == true)
		{		
			string noahFeaFileName = baseName + FEA_EXT;
			if(Disk::DoesFileExist(noahFeaFileName) == false)
			{
				printf("Writing - %s\n", noahFeaFileName.c_str());
				bool saveSuccess = SaveSift(noahFeaFileName.c_str(), imgShape);
				ENSURE(saveSuccess == true);
			}
		}
	}

	ENSURE(atleastOneFileLoaded == true);
	return true;
}


void FeatureSet::GetSetSortedByLocX(FeatureSet &targetSet)
{
	vector< std::pair<float, uint> > posIndices;
	posIndices.resize(this->size());
	for(uint iFea = 0; iFea < this->size(); iFea++)
	{
		posIndices[iFea] = std::make_pair(this->at(iFea).x, iFea);
	}
	std::sort(posIndices.begin(), posIndices.end());

	targetSet.clear();
	for(uint iFea = 0; iFea < this->size(); iFea++)
	{
		targetSet.push_back(this->at(posIndices[iFea].second));
	}
}

void FeatureSet::GetSetPrunedByDist(FeatureSet &targetSet, float dist)
{
	for(uint iFea = 1; iFea < this->size(); iFea++)
	{
		ENSURE(this->at(iFea - 1).x <= this->at(iFea).x);
	}

	float distSqr = dist * dist;

	targetSet.clear();
	for(uint iFea = 0; iFea < this->size(); iFea++)
	{
		bool pruneFea = false;
		Feature &currFea = this->at(iFea);
		for(int jFea = (int) targetSet.size() - 1; jFea >= 0; jFea--)
		{
			Feature &neighbourFea = targetSet[jFea];
			if(fabs(neighbourFea.x - currFea.x) <= dist)
			{
				float diff = neighbourFea.x - currFea.x;
				float currDistSqr = diff * diff;
				diff = neighbourFea.y - currFea.y;
				currDistSqr += diff * diff;

				if(currDistSqr <= distSqr)
				{
					pruneFea = true;
					break;
				}
			}
			else
			{
				break;
			}
		}		

		if(pruneFea == false)
		{	
			targetSet.push_back(currFea);
		}
	}
}

void FeatureMatches::Write(ofstream &outStream)
{
	outStream << "\n";
	outStream << this->size() << "\n";
	for(vector<FeatureMatch>::const_iterator currMatch = this->begin(); 
		currMatch != this->end();
		currMatch++)
	{
		outStream << currMatch->first << " " << currMatch->second << "\n";
	}
	outStream << "\n";
}

void FeatureMatches::Write(string fileName)
{
	ofstream outStream(fileName.c_str());
	ENSURE(outStream.is_open());
	Write(outStream);
	outStream.close();
}

void FeatureMatches::Read(ifstream &inStream, const FeatureSet *sourceFeatures, const FeatureSet *targetFeatures)
{
	ENSURE(this->size() == 0);

	int intType;

	inStream >> intType;
	ENSURE(intType >= 0);
	this->resize(intType);

	for(uint iMatch = 0; iMatch < this->size(); iMatch++)
	{
		inStream >> intType;
		ENSURE(intType >= 0);
		ENSURE(intType < (int) sourceFeatures->size());
		this->at(iMatch).first = intType;

		inStream >> intType;
		ENSURE(intType >= 0);
		ENSURE(intType < (int) targetFeatures->size());
		this->at(iMatch).second = intType;
	}
}
