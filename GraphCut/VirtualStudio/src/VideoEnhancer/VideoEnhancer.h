#ifndef __VIDEO_ENHANCER_H__
#define __VIDEO_ENHANCER_H__

#include "Definitions.h"
#include "ImageIO.h"
#include "ANN.h"

const int VE_Connectivity = 4;
const int VE_DX[VE_Connectivity] = {-1,  0,  1,  0};
const int VE_DY[VE_Connectivity] = { 0, -1,  0,  1};
const int VE_NEIGH_COMPLEMENT[VE_Connectivity] = {2, 3, 0, 1};

class VideoEnhancer
{
public:

	typedef struct _Label
	{
		unsigned short iTrain;
		unsigned short x;
		unsigned short y;

		_Label(unsigned short trainingImgIndex = USHRT_MAX, 
			   unsigned short xLoc = USHRT_MAX, 
			   unsigned short yLoc = USHRT_MAX)
		{
			this->iTrain = trainingImgIndex;
			this->x      = xLoc;
			this->y      = yLoc;
		}
	} Label;

	typedef CImageOf<Label *> CLabelPtrImg;

	typedef enum
	{
		EM_SUPER_RES,
		EM_FILTER
	} EnhancementMode;

	typedef struct _SuperResParams
	{
		float blurKernelStdDev;		
		int downSampleScale;
		float dogKernelStdDev;

		static _SuperResParams Default();
		void Dump(string prefix);
	} SuperResParams;

	typedef struct _VideoEnhancerParams
	{
		int sPatchSize;
		int lPatchSize;
		int sPatchOverlap;

		SuperResParams superResParams;

		double annErrorBound;
		int trainingDataSF; //SF = sampling frequence
		float matchKernelCornerWeight;
		int maxLabels;

		float matchEpsilon;
		float renderEpsilon;

		float dataCostNoiseStdDev;
		float spatialCostNoiseStdDev;

		EnhancementMode enhancementMode;

		int bpIters;

		string outPrefix;

		static _VideoEnhancerParams Default();
		void Dump(string prefix);

	} VideoEnhancerParams;


public:
	vector<CFloatImage> aImgsOrg;
	vector<CFloatImage> aPrimeImgsOrg;

	vector<CFloatImage> aImgs;
	vector<CFloatImage> aPrimeImgs;
	vector<CByteImage>  trainMasks;

	CFloatImage videoFrameOrg;
	CFloatImage videoFrame;

	CFloatImage matchKernel;

	CLabelPtrImg frameLabels;
	CDlbPtrImage frameDataCosts;

	CShape frameImgShape;
	CShape frameLabelsShape;

	ANNkd_tree    *searchTree;
	ANNpointArray dataPts;
	int dataPtsCount;

	VideoEnhancerParams params;

	int dataBands;
	int searchVecDim;

public:
	VideoEnhancer(VideoEnhancerParams videoEnhancerParams);
	~VideoEnhancer();

	void Enhance(vector<CFloatImage> &trainingImgs, CFloatImage &targetImg);

private:
	void initGlobalVars();
	void freeGlobalVars();

private: //VideoEnhancer-Matching.cpp
	void setupMatchKernel();
	void generateLabels(int x, int y, int width, int height, const vector<int> &trainingIndices);	

private: //VideoEnhancer-Training.cpp
	void createSuperResTrainData(vector<CFloatImage> &trainingImages);
	void createFilterTrainData(vector<CFloatImage> &aImgs, vector<CFloatImage> &aPrimeImgs);
	void createSearchTree(const vector<int> &trainingIndices);
	void destroySearchTree();

private: //VideoEnhancer-Data.cpp
	int getAuxElemCount();
	void createDataPtFromPatch(ANNpoint dataPt, CFloatImage &patch, const Label &dataPtLabel);
	Label extractLabelFromDataPt(int dataPtID);
	void getAPrimePatch(Label label);
	void normalizePatches(CFloatImage &aPatch, CFloatImage &aPrimePatch, float *mean, float *stdDevs);
	void normalizePatch(CFloatImage &aPatch, CFloatImage &aPrimePatch, float *mean, float *stdDevs);

public: //VideoEnhancer-SuperRes.cpp
	static CFloatImage GetCroppedImg(CFloatImage srcImg, int scale);

private: //VideoEnhancer-SuperRes.cpp
	CFloatImage createLowResImg(CFloatImage superResImg);
	CFloatImage createHighFreqImg(CFloatImage srcImg);


private: //VideoEnhancer-Render.cpp
	void generateResults();

};


#endif