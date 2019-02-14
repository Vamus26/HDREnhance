#ifndef __FLOW_H__
#define __FLOW_H__

#include "Definitions.h"
#include "Rig.h"
#include "CamFrameContext.h"
#include "Segmenter.h"
#include "Stereo.h"
#include "Transform-3x3.h"

const static float FLOW_OffScreen_Flowcost = 0.5f;
const static float FLOW_Occ_Flowcost = 0.5f;

const static float FLOW_OffScreen_DataCost = FLT_MAX;

class Flow
{
public:

	typedef struct _SegFlowData
	{
		int xOffset;
		int yOffset;

		_SegFlowData(int xOffset = INT_MAX, int yOffset = INT_MAX)
		{
			this->xOffset = xOffset;
			this->yOffset = yOffset;
		}
	} SegFlowData;

	typedef struct _FlowParams
	{
		bool processInMem;
		int searchWinSize;
		float noiseVar;
		float maxDataCost;

		static _FlowParams Default();
		void Dump(string prefix);

	} FlowParams;

private:
	FlowParams flowParams;
	Stereo::StereoParams stereoParams;
	Rig rig;
	vector<double>  depthPlanes;
	int             depthCount;

	View *currView;
	View *nextView;
	vector<View> currViewNeighs;
	vector<View> nextViewNeighs;

	Segmentation **currSegmentations;
	Segmentation **nextSegmentations;

public: //Flow.cpp

	Flow();
	~Flow();

	void Init(FlowParams flowParams, 
			  Stereo::StereoParams stereoParams,
			  Rig::RigParams rigParams);

	void ComputeFlow();

public:
	void computeFlow(int iCurrFrame, int iNextFrame);

private: //Flow-Load.cpp
	Segmentation* loadSegmentation(int iCam, int iFrame, Segmentation **segmentations);
	void unloadSegmentation(int iCam, int iFrame, Segmentation **segmentations);

	void loadFlowSegmentations(int iCurrFrame, int iNextFrame);
	void unloadFlowSegmentations(int iCurrFrame, int iNextFrame);

	void loadCamFlowContext(int iCam, int iCurrFrame, int iNextFrame);
	void unloadCurrCamFlowContext();

private: //Flow-Process.cpp
	void processCurrCamFlow();
	void processCurrViewSegFlow(Segment &seg);


	float getPixelFlowCost(int x, int y, int u, int v, const Segment &seg);
	float getPixelMatchCost(int x, int y, const CFloatImage &srcImg,
							int xx, int yy, const CFloatImage &targetImg,
							float noiseVar);


private: //Flow-DataCost.cpp
	CFloatImage getCurrViewDataCostImg(int offsetX, int offsetY);

	CFloatImage computeCurrViewDataCostImg(int offsetX, int offsetY);

	void getNeighViewsDataCost(const CVector3 &currUVZ, const CVector3 &nextUVZ, const CameraMats &currCamMats,
						       const Segment &currViewSeg, const Segment &nextViewSeg,
						       float *neighViewsDataCost, float *neighViewsDataCostWeight);


	float getPixelDataCost(int currX, int currY, int offsetX, int offsetY);

	float getPixelDataCost(float srcX, float srcY, const CFloatImage &srcImg,
						   float dstX, float dstY, const CFloatImage &dstImg)
	{
		return getPixelDataCostFast(srcX, srcY, srcImg, dstX, dstY, dstImg);
	}

	float getPixelDataCostBirch(float srcX, float srcY, const CFloatImage &srcImg,
						        float dstX, float dstY, const CFloatImage &dstImg);

	float getPixelDataCostFast(float srcX, float srcY, const CFloatImage &srcImg,
							   float dstX, float dstY, const CFloatImage &dstImg);


private: //Flow-Debug.cpp
	void getCurrViewFlowVis(CFloatImage &uVisImg, CFloatImage &vVisImg, float maxFlow);
	void visFlowProjections(int currX, int currY, int offsetX, int offsetY, bool flipY = true);
};

#endif