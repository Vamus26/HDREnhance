#ifndef __FEATURESET_H__
#define __FEATURESET_H__

#include "FeatureDefs.h"
#include "Image.h"

const string FEA_EXT = ".key";


class Feature {
public:
	typedef enum
	{ 
		SIFT_FEATURES = 0,
		HAR_FEATURES,
		HES_FEATURES,
		MSER_FEATURES		
	} FEATURE_TYPE;

	const static int    FeatureTypeCount = 4;
	static FEATURE_TYPE FeatureTypeList[FeatureTypeCount]; 
	static char         *FeatureTypeFileExt[FeatureTypeCount]; 
	class _InitFeatureTypeConstants
	{	
		_InitFeatureTypeConstants() 
		{
			Feature::FeatureTypeList[0]    = SIFT_FEATURES;
			Feature::FeatureTypeList[1]    = HAR_FEATURES;
			Feature::FeatureTypeList[2]    = HES_FEATURES;
			Feature::FeatureTypeList[3]    = MSER_FEATURES;
			Feature::FeatureTypeFileExt[0] = ".sift";
			Feature::FeatureTypeFileExt[1] = ".har";
			Feature::FeatureTypeFileExt[2] = ".hes";
			Feature::FeatureTypeFileExt[3] = ".mser";
		}
		static _InitFeatureTypeConstants init;
	};

	FEATURE_TYPE type;
	float x;
	float y;

	vector<uchar> data;
	static const uchar DataMin = 0;
	static const uchar DataMax = UCHAR_MAX;

public:
	Feature();
	bool read_sift(istream &is, CShape imgShape, float scale = 1.0f, 
				   const CByteImage &pruneMask = CByteImage(), bool useMask = false);

	void write_sift(ostream &os, CShape imgShape);

	bool read_Mikolajczyk(istream &is, CShape imgShape, FEATURE_TYPE featuretype, float scale = 1.0f,
						  const CByteImage &pruneMask = CByteImage(), bool useMask = false);

	bool readCustom(istream &is, CShape imgShape, float scale,
					const CByteImage &pruneMask = CByteImage(), bool useMask = false);

	void writeCustom(ofstream &f);
};

ostream &operator<<(ostream &os, const Feature &f);
istream &operator>>(istream &is, Feature &f);

class FeatureSet : public vector<Feature> {
	bool load_sift(const char *name, CShape imgShape, float scale = 1.0f,
				   const CByteImage &pruneMask = CByteImage(), bool useMask = false);

	bool load_Mikolajczyk(const char *name, CShape imgShape, Feature::FEATURE_TYPE featuretype, float scale = 1.0f,
						  const CByteImage &pruneMask = CByteImage(), bool useMask = false);

public:
	bool LoadAll(string baseName, CShape imgShape, float scale = 1.0f,
			 	 const CByteImage &pruneMask = CByteImage(), bool useMask = false);
	
	bool Load(const char *name, CShape imgShape, Feature::FEATURE_TYPE featuretype, float scale = 1.0f,
			  const CByteImage &pruneMask = CByteImage(), bool useMask = false);

	bool LoadCustom(const char *name, CShape imgShape, float scale,
		  		     const CByteImage &pruneMask = CByteImage(), bool useMask = false);

	bool LoadCustom(string name, CShape imgShape, float scale,
		  		     const CByteImage &pruneMask = CByteImage(), bool useMask = false)
	{
		return LoadCustom(name.c_str(), imgShape, scale,
						  pruneMask, useMask);				
	}


	bool SaveCustom(const char *name);
	bool SaveSift(const char *name, CShape imgShape);

	bool SaveCustom(string name)
	{
		return SaveCustom(name.c_str());
	}

	void GetSetSortedByLocX(FeatureSet &targetSet);

	void GetSetPrunedByDist(FeatureSet &targetSet, float dist);
};


class FeatureMatches : public vector<FeatureMatch>
{
public:
	void Write(ofstream &outStream);
	void Write(string fileName);	
	void Read(ifstream &inStream, const FeatureSet *sourceFeatures, const FeatureSet *targetFeatures);
};


#endif
