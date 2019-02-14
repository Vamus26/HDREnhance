#ifndef __SS_Setup_H__
#define __SS_Setup_H__

#include "Definitions.h"
#include "Rig.h"
#include "RigCalibrator.h"

class SS_Setup
{
public:
	typedef struct _SS_SetupParams
	{
		string targetDir;
		string optFN;
		string listFN;
		string noahBundleFN;
		int neighSpacing;
	    bool drewMode;

		static _SS_SetupParams Default();

		void Dump(const string prefix);
	} SS_SetupParams;

private:
	SS_SetupParams params;
	RigCalibrator::RigCalibratorParams rigCalibParams;
	vector<string> sceneFrameFNs;	
	vector<float>  sceneFrameScales;
	vector<string> camIDs;
	vector<string> camDirNames;

	Rig rig;

public: //SS_Setup.cpp
	void Start(SS_SetupParams ssSetupParams, RigCalibrator::RigCalibratorParams rcParams);

private: //SS_Setup.cpp
	void loadSceneFNs();
	void processCams();
	void calibrateScene();
	void writeOptFile();
};

#endif
