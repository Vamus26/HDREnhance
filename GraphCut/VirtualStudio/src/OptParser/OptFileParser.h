#ifndef __OPT_FILE_PARSER_H__
#define __OPT_FILE_PARSER_H__

#include <time.h>

#include "Definitions.h"
#include "Rig.h"
#include "Image.h"
#include "ImageIO.h"
#include "RigCalibrator.h"
#include "Stereo.h"
#include "SS_Setup.h"
#include "StereoReconstruction.h"
#include "GenericOpts.h"
#include "FeatureMatcher.h"
#include "MotionFromMatches.h"
#include "VideoSolver.h"

class OptFileParser
{
public:

	typedef enum
	{
		OP_DEFAULT = 0,
		OP_EXTRACT_SEQ_DATA,
		OP_CALIBRATE_RIG_CAMERAS,
		OP_GENERATE_RIG_FMATRICES,
		OP_OUTPUT_RIG_FRAME,
		OP_COMPUTE_STEREO,
		OP_BACK_UP_STEREO_BP,
		OP_RECONSTRUCT_VIDEO,
		OP_RECONSTRUCT_VIDEO_CR,
		OP_RECONSTRUCT_VIDEO_CC,
		OP_RECONSTRUCT_VIDEO_VS,
		OP_RECONSTRUCT_VIDEO_FNF,
		OP_COMPUTE_STEREO_ITER,
		OP_ADVANCE_STEREO_ITER,
		OP_COMPUTE_STEREO_RECONSTRUCTION,
		OP_COMPUTE_FLOW,
		OP_COMPUTE_FLOW_LARGE,
		OP_COMPUTE_FLOW_CROP,
		OP_SETUP_STATIC_SCENE,
		OP_FEA_COMBINE,
		OP_FEA_RENDER,
		OP_FEA_MATCH,
		OP_FEA_COMPUTE_FM,
		OP_FEA_RENDER_MATCHES,
		OP_FEA_RENDER_FM,
		OP_GENERIC
	} AppOperation;

	typedef struct _OptFileParams
	{
		int seed;
		AppOperation appOP;
		bool overwriteResults;

		Rig::RigParams rigParams;
		RigCalibrator::RigCalibratorParams rigCalibratorParams;
		Stereo::StereoParams stereoParams;
		StereoReconstruction::SR_Params srParams;
		SS_Setup::SS_SetupParams ssSetupParams;
		GenericOpts genericParams;
		FeatureMatcher::FeatureMatcherParams feaMatcherParams;
		MotionFromMatches::MotionFromMatchesParams mfmParams;
		VideoSolver::VideoSolverParams vsParams;

		static _OptFileParams Default();

		void Dump();
	} OptFileParams;
 
	OptFileParams Load(const char *optionsFN, char* argv[], bool dumpOptionsRead = true);

private:

	bool readInt(       string option, int             &intTarget,    string optStr, vector<string> &optVals);
	bool readUint(      string option, uint            &uintTarget,   string optStr, vector<string> &optVals);
	bool readFloat(     string option, float           &floatTarget,  string optStr, vector<string> &optVals);
	bool readDouble(    string option, double          &doubleTarget, string optStr, vector<string> &optVals);
	bool addString(     string option, vector<string>  &strArray,     string optStr, vector<string> &optVals);
	bool addInt(        string option, vector<int>     &intArray,     string optStr, vector<string> &optVals);
	bool addFloat(      string option, vector<float>   &floatArray,   string optStr, vector<string> &optVals);
	bool addBool(       string option, vector<bool>    &boolArray,    string optStr, vector<string> &optVals);
	bool addDirName(    string option, vector<string>  &strArray,     string optStr, vector<string> &optVals);
	bool addCamPair(    string option, vector<CamPair> &camPairArray, string optStr, vector<string> &optVals);
	bool readString(    string option, string          &strTarget,    string optStr, vector<string> &optVals);
	bool readDirName(   string option, string          &strTarget,    string optStr, vector<string> &optVals);
	bool readBool(      string option, bool            &boolTarget,   string optStr, vector<string> &optVals);
	bool readAppOp(		string option, AppOperation    &appOP,        string optStr, vector<string> &optVals);
	bool readImgFilter(	string option, ImageFilter     &imGFilter,    string optStr, vector<string> &optVals);
	
	bool addMV_Correspondances( string option, vector<MV_Correspondances> &mvcList, string optStr, vector<string> &optVals);
	bool addTrack( string option, vector<Track> &tracks, string optStr, vector<string> &optVals);
	
	void neutralizePaths(string &line);

	bool readRigParams(OptFileParams &params, string optStr, vector<string> lineTokens);
	bool readRigCalibratorParams(OptFileParams &params, string optStr, vector<string> lineTokens);
	bool readStereoParams(OptFileParams &params, string optStr, vector<string> lineTokens);
	bool readStereoReconstructionParams(OptFileParams &params, string optStr, vector<string> lineTokens);
	bool readSS_SetupParams(OptFileParams &params, string optStr, vector<string> lineTokens);
	bool readSegmenterParams(OptFileParams &params, string optStr, vector<string> lineTokens);
	bool readGenericParams(OptFileParams &params, string optStr, vector<string> lineTokens);
	bool readFeaMatcherParams(OptFileParams &params, string optStr, vector<string> lineTokens);
	bool readMFM_Params(OptFileParams &params, string optStr, vector<string> lineTokens);
	bool readPC_SegmenterParams(OptFileParams &params, string optStr, vector<string> lineTokens);
	bool readVideoSolverParams(OptFileParams &params, string optStr, vector<string> lineTokens);
};


#endif
