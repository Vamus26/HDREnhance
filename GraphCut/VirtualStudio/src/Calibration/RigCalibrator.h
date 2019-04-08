#ifndef __RIG_CALIBRATOR_H__
#define __RIG_CALIBRATOR_H__

#include "Definitions.h"
#include "Rig.h"
#include "CameraMats.h"

const static string RIG_CALIBRATOR_DrewFileHeader = "drews";

class RigCalibrator
{
public:
	const static int MIN_POINTS_FOR_CALIB = 288;

	typedef struct _RigCalibratorParams
	{
		string refCamID;
		FrameSeq frameSeqToSearch;
		int framesToFind;
		int sharpenAmount;
		int maxSharpenIters;

		bool useNoahCalib;
		bool useDrewCalib;
		bool useLarryCalib;
		int minViewsForScenePoint;

		CVector3 pcLowerBound;
		CVector3 pcUpperBound;
		string calibFN;

		static _RigCalibratorParams Default();

		void Dump(string prefix);
	} RigCalibratorParams;

private:
	Rig *rig;
	string calibDirName;
	RigCalibratorParams params;
	vector< vector<CameraMats> > frameCameraMats;

public:
	RigCalibrator(Rig *rigToCalibrate);

	void Calibrate(RigCalibratorParams rigCalibParams);

private: //RigCalibrator-NoahLarry.cpp
	void loadNoahDrewCalibFile(int minObservationsForPoint, bool drewMode);
	void loadLarryCalibFile();

private: //RigCalibrator-CamMatrices.cpp
	void genCamMats();
	void loadCamMats();
	void writeFinalCamMats();
	void recalMatsUsingRefCam();

private: //RigCalibrator-PointExtraction.cpp
	void findValidFrames();
	void writeExtractPointsOptFile(string calibImageName, int iCam, int iFrame);
	bool extractPoints(string calibImageName, int iCam, int iFrame,
					   int *pointsFound);	
	bool extractPoints(string calibImageName, int iCam, int iFrame, 
					   int maxSharpenIters);

private: //RigCalibrator-FileNames.cpp
	string getExtractPtBinFN();
	string getCalibDirName();
	string getCalibImageName(int camIndex, int validFrameIndex);
	string getExtractPtOptFN();
	string getExtractPtOutFN();
	string getPointsFileName(int camIndex, int validFrameIndex);
	string getModelFileName(int camIndex,  int validFrameIndex);
	string getCalibResultFN(int iCam);
	string getCalibCmdPrefix(int iCam);
};

#endif
