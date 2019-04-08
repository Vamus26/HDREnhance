#ifndef __DRIVER_H__
#define __DRIVER_H__

#include "OptFileParser.h"
#include "FeatureDefs.h"

class Driver
{
	OptFileParser::OptFileParams params;

public: //Driver.cpp
	Driver(OptFileParser::OptFileParams driverParams);
	void Start();

private: //Driver.cpp
	void outputRigFrame();
	void outputRigFMatrices();

private:  //Driver-PreProcess.cpp
	void extractSeqData();
	void calibrateRigCams();
	void setupStaticScene();

private:  //Driver-Stereo.cpp
	void computeStereo();
	void backUpStereoBP_Data();
	void computeStereoIter();
	void advanceStereoIter();
	void computeStereoReconstruction();
	void reconstructVideo();
	void reconstructVideoFNF();
	

private: //Driver-Features.cpp
	void combineFeatures();
	void matchFeatures();
	void computeFeaFM();
	void computeWarpHM();
	void loadFeatureMatches(string img1ID, string img2ID, 
						    FeatureSet &img1FeaSet, FeatureSet &img2FeaSet,
						    FeatureMatches &feaMatches,
							FeatureMatcher::FeatureMatcherParams feaMatcherParams);
	void loadFeatureSet(string imgID, FeatureSet &featureSet);

	string getMatchesFN(string img1ID, string img2ID)
	{
		return img1ID + "-" + Disk::GetBaseFileName(img2ID) + ".matches";
	}

private: //Driver-Features-Debug.cpp
	void renderFeatures();
	void renderFeatureMatches();
	void renderFM_Inliers();

private: //Driver-Flow.cpp
	void computeFlow(bool computeLargeFlow);
	void computeFlowCrop();
	void computeBlackFlow(bool computeLargeFlow);
	void computeBlackFlowCrop();
	void computeStereoFlow();
	void computeStereoFlowCrop();	

private: //Driver-Generic.cpp
	void equalizeDataColors(bool equalizeVideoColor);
	void execGenericOp();
	void generateLargeImages();
	void genSmoothCamPath();
	void generateCropedScaledVideo();	
	void getCamScaleCrop(int iCam, int int1ListOffset, CShape camFrameShape, CShape vidFrameShape,
		                 float &scale, int &cropX, int &cropY, int &cropW, int &cropH, bool invertYaxis = true);
	void checkFrameCropParams(int int1ListExtraParamCount);
};

#endif
