#ifndef __STEREO_RECONSTRUCTION_H__
#define __STEREO_RECONSTRUCTION_H__

#include "Stereo.h"
#include "RegistrationDefs.h"
#include "PiecesBlender.h"
#include "BlendSolver.h"
#include "Transform-2x2.h"

typedef struct _ZPixel
{
	Vector3<float> color;
	Vector3<float> analogyColor;
	float weight;
	float depth;
	short planeIndex;
} ZPixel;

typedef struct _SRS_Context //Stereo Reconstruction Source Context
{
	Camera sourceCam;
	int iSourceCam;
	CFloatImage sourceImg;
	CFloatImage sourceAnalogyImg;
	CShortImage sourcePlaneMap;
	CShortImage sourceLabelMap;
	CByteImage  sourceMask;
	Segmentation *sourceSeg;

	float scale;
	CVector2 cropOrigin;
	CVector2 cropExt;

	_SRS_Context()
	{
		this->scale = 1.0f;
		this->cropOrigin = this->cropExt = CVector2::Null();
	}
} SRS_Context;

class StereoReconstruction : public Stereo
{
public:
	typedef struct _SR_Params //Stereo Reconstruction params
	{
		int iFrame;
		string targetCamID;
		vector<string> srsCamIDs;
		vector<float> srsCamScales;
		vector<bool>  srsCamFillerOnly;
		int depthMatchThres;
		int depthDisConMaskThres;
		int depthDisConMaskErWidth;
		PiecesBlender::PiecesBlenderParams pbParams;
		BlendSolver::BlendSolverParams bsParams;
		bool reconstructUsingAnalogy;
		bool useAnalogyMask;

		float targetScale;
		CVector2 targetCropOrigin;
		CVector2 targetCropExt;

		int mattingRad;

		vector<string> neighCamIDs;
		vector<float> neighCamScales;
		vector<CVector2> neighCamCropOrigins;
		vector<CVector2> neighCamCropExts;

		static _SR_Params Default();
		void Dump(string prefix);
	} SR_Params;

private:
	SR_Params srParams;
	SRS_Context targetContextSRS;
	CameraMats  refCamMats;

	vector<CFloatImage> neighProjImgs;
	vector<CByteImage>  neighProjMasks;

public: //StereoReconstruction.cpp
	void Reconstruct(SR_Params reconstructionParams, bool overwriteResults);
	void ComputeMatting(SR_Params reconstructionParams, bool overwriteResults);
	void ReconstructMask(SR_Params reconstructionParams, bool overwriteResults);

private: //StereoReconstruction.cpp
	void reconstruct();

	void reconstructMask();		

	void updateFillerSRS_Masks(RegisteredPieces &regPieces);

	void computeSRS_Mask(SRS_Context &srsContext);

	int getFrontoPlaneIndex(double x, double y, const SRS_Context &srsContext, int planeIndex, 
							bool returnPlaneIndexInTargetSpace, const SRS_Context &targetContext = SRS_Context());

	RegisteredPiece createRegisteredPiece(SRS_Context &srcSRS, SRS_Context &targetSRS, bool useForwardProjection);

	void computeProjData(SRS_Context &srcSRS, SRS_Context &targetSRS,
						 CFloatImage &projImg, CFloatImage &projAnalogyImg, CByteImage &projMask);

	void pcMaskPruner(RegisteredPiece &regPiece);

public: //StereoReconstruction-NVI.cpp
	void ReconstructNV(SR_Params reconstructionParams, bool overwriteResults);

private: //StereoReconstruction-NVI.cpp
	void reconstructNV();

	void reconstructNV_Median(CByteImage &reconstructionMask, 
							  CByteImage &medianReconImg, CByteImage &medianReconImgMask, CShortImage &medianPlaneMap);

	void reconstructNV_MedianUsingGC(CByteImage &reconstructionMask, 
									 CByteImage &medianReconImg, CByteImage &medianReconImgMask, CShortImage &medianPlaneMap);

	void createViewProjection(SRS_Context &srsContext, SRS_Context &targetContext, bool matchTargetPlaneMap,
							  CFloatImage &projImg, CFloatImage &projAnalogyImg, CByteImage &projMask, CShortImage &projPlanes);
	
	
	int extractProjDataFromZBuffer(const vector< vector<ZPixel> > &zBuffer, SRS_Context &targetContext, bool matchTargetPlaneMap,
								   CFloatImage &projImg, CFloatImage &projAnalogyImg, CByteImage &projMask, CShortImage &projPlanes);

	CByteImage constructDepthRangeMask(SRS_Context &srsContext, CVector3 realWorldMin, CVector3 realWorldMax);

private: //StereoReconstruction-FileNames.cpp
	string getTargetContextRegFN();	
	string getTargetContextBlendFN();
	string getTargetContextMaskFN();
	string getTargetContextNovelViewFN();

public:  //StereoReconstruction-FileNames.cpp
	static string GetTargetContextBlendFN(const Camera *cam, int iFrame, string outDir);
	static string GetTargetContextRegFN(const Camera *cam, int iFrame, string outDir);
	static string GetTargetContextMaskFN(const Camera *cam, int iFrame, string outDir);
	static string GetTargetContextNovelViewFN(const Camera *cam, int iFrame, string outDir);

private: //StereoReconstruction-IO.cpp
	void loadContext(bool loadSegData);
	void loadAnalogyMask(SRS_Context &srsContext);
	void loadNeighContext();
	void unloadContext();
	void unloadSRS_Context(SRS_Context &srsContext);

	SRS_Context loadSRS_Context(string camID, bool loadSegData,
								bool computeDisConMask, bool loadAnalogy,
								const CameraMats &refCameraMats, bool recenterSRS_Cam,
								float scale = 1.0f, CVector2 cropOrigin = CVector2::Null(), CVector2 cropExt = CVector2::Null());

public: //StereoReconstruction-Flow.cpp
	void ComputeFlow(int srcCamIndex, int targetCamIndex, 
		             CFloatImage &flowFieldImg, SR_Params reconstructionParams);

	void ComputeFlow(int srcCamIndex, int targetCamIndex, 
		             CFloatImage &flowFieldImg, SR_Params reconstructionParams,
					 float srcScale,    CVector2 srcCropOrigin, CVector2 srcCropExt,
					 float targetScale, CVector2 targetCropOrigin, CVector2 targetCropExt);

public: //StereoReconstruction-Matte.cpp
	void computeMatte(SRS_Context &srsContext, SR_Params reconstructionParams);
	void computeColorsFB(SRS_Context &srsContext, 
						 CFloatImage &fgImg, CFloatImage &bgImg,
						 CShortImage &fgFrontoPlaneMap, CShortImage &bgFrontoPlaneMap,
						 CFloatImage &imgAlpha, CFloatImage &imgDistToBG,
						 SR_Params srParams);
};

#endif

