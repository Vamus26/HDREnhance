#ifndef __SUPER_RES_H__
#define __SUPER_RES_H__

#include "Definitions.h"
#include "Image.h"

#include "ANN.h"

const float SuperRes_MinMean = 0.01f;
//bug
const float SuperRes_MinDataCostDist = 1.0f;
const float SuperRes_MinSpatialSmoothnessCostDist = 1.0f;


const float SuperRes_MatchEpsilon = 0.1f;
const float SuperRes_RenderEpsilon = 0.1f;


const int BP_Connectivity = 4;
const int BP_DX[BP_Connectivity] = {-1,  0,  1,  0};
const int BP_DY[BP_Connectivity] = { 0, -1,  0,  1};
const int BP_NEIGH_COMPLEMENT[BP_Connectivity] = {2, 3, 0, 1};

typedef enum
{
	BT_LEFT = 0,
	BT_BOTTOM,
	BT_RIGHT,
	BT_TOP	
} BoundaryType;

class SuperRes
{
public:

	typedef struct _SuperResParams
	{
		int patchSize;
		int largePatchSize;

		int patchOverlap;

		float blurKernelStdDev;		
		int downSampleScale;

		double annErrorBound;

		int maxLabels;

		float dogKernelStdDev;

		float dataCostNoiseStdDev;
		float spatialCostNoiseStdDev;

		bool normalizeMeanStdDev;

		int bpIters;

		string outPrefix;

		static _SuperResParams Default();
		void Dump(string prefix);
	} SuperResParams;


public:

	vector<CShape> trainingShapes;

	vector<CFloatImage> targetImgs;
	vector<CIntPtrImage> labels;
	vector<CDlbPtrImage> datacosts;

	vector<CFloatImage> stdImgs;

	vector<CFltPtrImage> spatialMinDists;
	vector<CFltPtrImage> spatialProbSum;

	vector<CFltPtrPtrImage> spatialMsges;
	vector<CFltPtrPtrImage> prevSpatialMsges;

	vector<CFloatImage> aImgsOrg;
	vector<CFloatImage> aPrimeImgsOrg;

	vector<CFloatImage> aImgs;
	vector<CFloatImage> aPrimeImgs;
	
	CShape imgShape;
	CShape labelShape;

	ANNkd_tree    *searchTree;
	ANNpointArray dataPts;

	ANNkd_tree    *searchTree2;

	SuperResParams params;

	CFloatImage flowField;
	CFloatImage warpResult;
	bool usePrevResult;

	CFloatImage matchKernel;

public: //SuperRes-Training.cpp
	SuperRes(SuperResParams superResParams);

	void Train(vector<CFloatImage> &aImgs, vector<CFloatImage> &aPrimeImgs);	
	void TrainSuperRes(vector<CFloatImage> &aImgs);
	void TrainFilter(vector<CFloatImage> &aImgs, vector<CFloatImage> &aPrimeImgs);



private: //SuperRes-Training.h
	
	void createSearchTree(vector<CFloatImage> &aImgs, vector<CFloatImage> &aPrimeImgs);
	CFloatImage createLowResImg(CFloatImage superResImg);
	CFloatImage createHighFreqImg(CFloatImage srcImg);
	
public: //SuperRes.cpp
	void SetTargetImg(CFloatImage targetImg);

	void ComputeSuperRes(CFloatImage targetImg);
	void ComputeFilter(CFloatImage targetImg);
	void UsePrevResult(CFloatImage &flowField, CFloatImage &warpResult);

	static CFloatImage GetCroppedImg(CFloatImage srcImg, int scale);

	void VisLabels(CFloatImage targetImg, int x, int y);

public:
	void start();

	void setupMatchKernel();

	CFloatImage getLowResPatch(int patchID);
	CFloatImage getLowResPatch(int patchID, int patchSize);
	CFloatImage getHiResPatch(int patchID);	
	CFloatImage getLowResLargePatch(int patchID);
	CFloatImage getHiResLargePatch(int patchID);
	void getPatchLoc(int patchID, int &x, int &y, int &iData);

	void createLowResLargePatch(CFloatImage &patch, ANNpoint annPoint);

	void getPatchStatsIncr(int patchID, float *meanIncr, float *stdDevIncr);

	void initGlobalVars();
	void freeGlobalVars();

	void generateLabels(int iFrame);

	void augmentDataCosts(int iFrame);

	void normalizeDataCosts(int iFrame);

	void computeDataCosts(int iFrame, int patchSize, bool normalize, float newDatacostW = 0.5f);

	float getPatchDist(CFloatImage &currPatch, int patchID, int patchSize);

	void computeDataCosts(int iFrame);

	float computeSpatialSmoothnessDist(int patchID1, int patchID2, BoundaryType boundaryType, uint patchTargetAddr1, uint patchTargetAddr2, int iFrame);

	void generateResults(int iFrame, bool standAloneResult);

    void computeSpatialSmoothnessData(int iFrame);

public: //SuperRes-BP.cpp
	void bpIter(int iFrame);

	void computeSpatialMsg(uint nodeAddr, uint neighNodeAddr, int iFrame, int iNeigh);

	int getLabel(int iFrame, int nodeAddr);
};

#endif