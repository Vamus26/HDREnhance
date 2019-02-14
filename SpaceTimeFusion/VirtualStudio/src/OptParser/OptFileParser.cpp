#include "OptFileParser.h"
#include <fstream>

#pragma warning(disable: 4127)

bool OptFileParser::addTrack( string option, vector<Track> &tracks, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) != 0)
		return false;
	{
		int currOptIndex = 0;

		ENSURE((int) optVals.size() > currOptIndex);
		int numOfPoints = -1; 
		sscanf(optVals[currOptIndex].c_str(), "%i", &numOfPoints);
		INSIST(numOfPoints > 1);
		currOptIndex++;

		Track newTrack;

		ENSURE((int) optVals.size() > currOptIndex);
		newTrack.planeID = -1; 
		sscanf(optVals[currOptIndex].c_str(), "%i", &newTrack.planeID);
		INSIST(newTrack.planeID != -1);
		currOptIndex++;

		ENSURE((int) optVals.size() == currOptIndex + (3 * numOfPoints));

		for(int iPoint = 0; iPoint < numOfPoints; iPoint++)
		{
			string camID = optVals[currOptIndex];
			newTrack.camIDs.push_back(camID);
			currOptIndex++;

			CVector2 trackLoc(-1.0, -1.0);
			sscanf(optVals[currOptIndex].c_str(), "%lf", &(trackLoc[0]));
			INSIST(trackLoc[0] >= 0.0);
			currOptIndex++;
			sscanf(optVals[currOptIndex].c_str(), "%lf", &(trackLoc[1]));
			INSIST(trackLoc[1] >= 0.0);
			currOptIndex++;

			newTrack.points.push_back(trackLoc);
		}

		tracks.push_back(newTrack);

		return true;
	}
}

bool OptFileParser::addMV_Correspondances( string option, vector<MV_Correspondances> &mvcList, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) != 0)
		return false;
	{
		int currOptIndex = 0;

		ENSURE((int) optVals.size() > currOptIndex);
		int numOfPoints = -1; 
		sscanf(optVals[currOptIndex].c_str(), "%i", &numOfPoints);
		INSIST(numOfPoints > 0);
		currOptIndex++;

		MV_Correspondances mvCorrespondances;

		for(int iPoint = 0; iPoint < numOfPoints; iPoint++)
		{
			INSIST((int) optVals.size() > currOptIndex);
			int numOfViews = -1;
			sscanf(optVals[currOptIndex].c_str(), "%i", &numOfViews);
			INSIST(numOfViews > 0);
			currOptIndex++;

			INSIST((int) optVals.size() >= currOptIndex + (3 * numOfViews));
			MV_Correspondance mvCorrespondance;
			for(int iView = 0; iView < numOfViews; iView++)
			{
				string camID = optVals[currOptIndex];
				mvCorrespondance.camIDs.push_back(camID);
				currOptIndex++;

				CVector2 viewPoint(-1.0, -1.0);

				sscanf(optVals[currOptIndex].c_str(), "%lf", &(viewPoint[0]));
				INSIST(viewPoint[0] >= 0.0);
				currOptIndex++;

				sscanf(optVals[currOptIndex].c_str(), "%lf", &(viewPoint[1]));
				INSIST(viewPoint[1] >= 0.0);
				currOptIndex++;

				mvCorrespondance.points.push_back(viewPoint);
			}

			mvCorrespondances.push_back(mvCorrespondance);
		}

		mvcList.push_back(mvCorrespondances);

		return true;
	}
}

bool OptFileParser::readImgFilter(string option, ImageFilter &imgFilter, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{
		ENSURE(optVals.size() == 1);

		if(optVals[0].compare("IF_NO_OP") == 0)
		{
			imgFilter = IF_NO_OP;
		}
		else if(optVals[0].compare("IF_LAB_COLOR_TRANSFER") == 0)
		{
			imgFilter = IF_LAB_COLOR_TRANSFER;
		}
		else
		{
			REPORT_FAILURE("Unknown Image Filter - %s.", optVals[0].c_str());
		}
		
		return true;
	}

	return false;    
}

bool OptFileParser::readAppOp(string option, AppOperation &appOP, string optStr, vector<string> &optVals)
{	
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);

		if(optVals[0].compare("OP_EXTRACT_SEQ_DATA") == 0)
		{
			appOP = OP_EXTRACT_SEQ_DATA;
		}
		else if(optVals[0].compare("OP_CALIBRATE_RIG_CAMERAS") == 0)
		{
			appOP = OP_CALIBRATE_RIG_CAMERAS;
		}
		else if(optVals[0].compare("OP_OUTPUT_RIG_FRAME") == 0)
		{
			appOP = OP_OUTPUT_RIG_FRAME;
		}
		else if(optVals[0].compare("OP_GENERATE_RIG_FMATRICES") == 0)
		{
			appOP = OP_GENERATE_RIG_FMATRICES;
		}		
		else if(optVals[0].compare("OP_COMPUTE_STEREO") == 0)
		{
			appOP = OP_COMPUTE_STEREO;
		}
		else if(optVals[0].compare("OP_BACK_UP_STEREO_BP") == 0)
		{
			appOP = OP_BACK_UP_STEREO_BP;
		}
		else if(optVals[0].compare("OP_COMPUTE_STEREO_ITER") == 0)
		{
			appOP = OP_COMPUTE_STEREO_ITER;
		}
		else if(optVals[0].compare("OP_ADVANCE_STEREO_ITER") == 0)
		{
			appOP = OP_ADVANCE_STEREO_ITER;
		}
		else if(optVals[0].compare("OP_COMPUTE_STEREO_RECONSTRUCTION") == 0)
		{
			appOP = OP_COMPUTE_STEREO_RECONSTRUCTION;
		}
		else if(optVals[0].compare("OP_COMPUTE_FLOW") == 0)
		{
			appOP = OP_COMPUTE_FLOW;
		}
		else if(optVals[0].compare("OP_COMPUTE_FLOW_LARGE") == 0)
		{
			appOP = OP_COMPUTE_FLOW_LARGE;
		}
		else if(optVals[0].compare("OP_COMPUTE_FLOW_CROP") == 0)
		{
			appOP = OP_COMPUTE_FLOW_CROP;
		}
		else if(optVals[0].compare("OP_GENERIC") == 0)
		{
			appOP = OP_GENERIC;
		}
		else if(optVals[0].compare("OP_SETUP_STATIC_SCENE") == 0)
		{
			appOP = OP_SETUP_STATIC_SCENE;
		}
		else if(optVals[0].compare("OP_FEA_COMBINE") == 0)
		{
			appOP = OP_FEA_COMBINE;
		}
		else if(optVals[0].compare("OP_FEA_RENDER") == 0)
		{
			appOP = OP_FEA_RENDER;
		}
		else if(optVals[0].compare("OP_FEA_MATCH") == 0)
		{
			appOP = OP_FEA_MATCH;
		}
		else if(optVals[0].compare("OP_FEA_RENDER_MATCHES") == 0)
		{
			appOP = OP_FEA_RENDER_MATCHES;
		}
		else if(optVals[0].compare("OP_FEA_RENDER_FM") == 0)
		{
			appOP = OP_FEA_RENDER_FM;
		}
		else if(optVals[0].compare("OP_FEA_COMPUTE_FM") == 0)
		{
			appOP = OP_FEA_COMPUTE_FM;
		}
		else if(optVals[0].compare("OP_RECONSTRUCT_VIDEO") == 0)
		{
			appOP = OP_RECONSTRUCT_VIDEO;
		}
		else if(optVals[0].compare("OP_RECONSTRUCT_VIDEO_CR") == 0)
		{
			appOP = OP_RECONSTRUCT_VIDEO_CR;
		}
		else if(optVals[0].compare("OP_RECONSTRUCT_VIDEO_VS") == 0)
		{
			appOP = OP_RECONSTRUCT_VIDEO_VS;
		}
		else if(optVals[0].compare("OP_RECONSTRUCT_VIDEO_FNF") == 0)
		{
			appOP = OP_RECONSTRUCT_VIDEO_FNF;
		}		
		else if(optVals[0].compare("OP_RECONSTRUCT_VIDEO_CC") == 0)
		{
			appOP = OP_RECONSTRUCT_VIDEO_CC;
		}
		else
		{
			REPORT_FAILURE("Unknown app operation - %s.", optVals[0].c_str());
		}
		
		return true;
	}

	return false;
}


bool OptFileParser::readBool(string option, bool &boolTarget, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);

		if(optVals[0].compare("true") == 0)
		{
			boolTarget = true;
		}
		else if(optVals[0].compare("false") == 0)
		{
			boolTarget = false;
		}
		else
		{
			REPORT_FAILURE("Invalid bool val - %s", optVals[0].c_str());

		}
		return true;
	}

	return false;
}

bool OptFileParser::readInt(string option, int &intTarget, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);
		int matched = sscanf(optVals[0].c_str(), "%i", &intTarget);
		ENSURE(matched == 1);

		return true;
	}

	return false;
}

bool OptFileParser::readUint(string option, uint &uintTarget, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);
		int matched = sscanf(optVals[0].c_str(), "%u", &uintTarget);
		ENSURE(matched == 1);

		return true;
	}

	return false;
}


bool OptFileParser::readFloat(string option, float &floatTarget, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);
		int matched = sscanf(optVals[0].c_str(), "%f", &floatTarget);
		ENSURE(matched == 1);

		return true;
	}

	return false;
}

bool OptFileParser::readDouble(string option, double &doubleTarget, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);
		int matched = sscanf(optVals[0].c_str(), "%lf", &doubleTarget);
		ENSURE(matched == 1);

		return true;
	}

	return false;
}


bool OptFileParser::addString(string option, vector<string> &strArray, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);
		strArray.push_back(optVals[0]);
		return true;
	}

	return false;
}

bool OptFileParser::addInt(string option, vector<int> &intArray, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);
		int param;
		int matched = sscanf(optVals[0].c_str(), "%i", &param);
		ENSURE(matched == 1);
		intArray.push_back(param);
		return true;
	}

	return false;
}


bool OptFileParser::addBool(string option, vector<bool> &boolArray, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);
		bool boolVal;
		bool matched = readBool(option, boolVal, optStr, optVals);
		ENSURE(matched == true);
		boolArray.push_back(boolVal);
		return true;
	}

	return false;
}

bool OptFileParser::addFloat(string option, vector<float> &floatArray, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);
		float param;
		int matched = sscanf(optVals[0].c_str(), "%f", &param);
		ENSURE(matched == 1);
		floatArray.push_back(param);
		return true;
	}

	return false;
}

bool OptFileParser::addCamPair(string option, vector<CamPair> &camPairArray, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 3);
		CamPair newPair;
		newPair.cam1ID = optVals[0];
		newPair.cam2ID = optVals[1];
		newPair.dist   = (float) atof(optVals[2].c_str());
		camPairArray.push_back(newPair);
		return true;
	}

	return false;
}

bool OptFileParser::addDirName(string option, vector<string> &strArray, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);
		string dirStr = optVals[0];
		if(dirStr[dirStr.size() - 1] != OS_PATH_DELIMIT)
		{
			dirStr += OS_PATH_DELIMIT_STR;
		}		
		strArray.push_back(dirStr);
		return true;
	}

	return false;
}




bool OptFileParser::readString(string option, string &strTarget, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);
		strTarget = optVals[0];
		return true;
	}

	return false;
}

bool OptFileParser::readDirName(string option, string &strTarget, string optStr, vector<string> &optVals)
{
	if(optStr.compare(option) == 0)
	{			
		ENSURE(optVals.size() == 1);
		strTarget = optVals[0];
		if(strTarget[strTarget.size() - 1] != OS_PATH_DELIMIT)
		{
			strTarget += OS_PATH_DELIMIT_STR;
		}		
		return true;
	}

	return false;
}


void OptFileParser::neutralizePaths(string &line)
{
	for(uint i = 0; i < line.length(); i++)
	{
		if(line[i] == WRONG_OS_PATH_DELIMIT)
		{
			line[i] = OS_PATH_DELIMIT;
		}
	}
}


OptFileParser::OptFileParams OptFileParser::Load(const char* optionsFileName, char* argv[], bool dumpOptionsRead)
{
	OptFileParams params = OptFileParams::Default();

	vector<string> optStrings;

	ifstream inputStream(optionsFileName);
	ENSURE(inputStream.is_open());
	string lineBuff;
	while(std::getline(inputStream, lineBuff))
	{
		optStrings.push_back(lineBuff);
	}
	inputStream.close();

	int argI = 0;
	while(argv[argI] != NULL)
	{
		char charBuff[1024];
		sprintf(charBuff, "%s", argv[argI]);
		string lineBuff(charBuff);
		optStrings.push_back(lineBuff);
		argI++;
	}
    	
	for(uint iOptStr = 0; iOptStr < optStrings.size(); iOptStr++)
	{
		vector<string> lineTokens;	
		string lineBuff = optStrings[iOptStr];

		neutralizePaths(lineBuff);

		if(lineBuff.size() == 0) continue;
		if(lineBuff[0] == '#')   continue;

		lineTokens.clear();
		Tokenize(lineBuff, lineTokens, " \t=");
		if(lineTokens.size() == 0) continue;

		if(lineTokens.size() < 2)
		{
			PRINT_INT(iOptStr);
			PRINT_STRING(lineTokens[0]);
			ENSURE(lineTokens.size() >= 2);
		}		

		string optStr = lineTokens[0];
		lineTokens.erase(lineTokens.begin(), lineTokens.begin() + 1);

		bool optStrMatched = false;

		optStrMatched |= readInt(   "seed",             params.seed,  optStr, lineTokens);
		optStrMatched |= readAppOp( "appOP",            params.appOP, optStr, lineTokens);
		optStrMatched |= readBool(  "overwriteResults", params.overwriteResults, optStr, lineTokens);
		
		if(optStrMatched == true) continue;

		optStrMatched |= readGenericParams(params, optStr, lineTokens);
		if(optStrMatched == true) continue;

		optStrMatched |= readMFM_Params(params, optStr, lineTokens);
		if(optStrMatched == true) continue;

		optStrMatched |= readFeaMatcherParams(params, optStr, lineTokens);
		if(optStrMatched == true) continue;

		optStrMatched |= readRigParams(params, optStr, lineTokens);
		if(optStrMatched == true) continue;

		optStrMatched |= readRigCalibratorParams(params, optStr, lineTokens);
		if(optStrMatched == true) continue;

		optStrMatched |= readStereoParams(params, optStr, lineTokens);
		if(optStrMatched == true) continue;

		optStrMatched |= readStereoReconstructionParams(params, optStr, lineTokens);
		if(optStrMatched == true) continue;

		optStrMatched |= readSS_SetupParams(params, optStr, lineTokens);
		if(optStrMatched == true) continue;

		optStrMatched |= readSegmenterParams(params, optStr, lineTokens);
		if(optStrMatched == true) continue;		

		optStrMatched |= readPC_SegmenterParams(params, optStr, lineTokens);
		if(optStrMatched == true) continue;	

		optStrMatched |= readVideoSolverParams(params, optStr, lineTokens);
		if(optStrMatched == true) continue;	

		if(optStrMatched == false)
		{
			REPORT_FAILURE("Unknown option: %s\n", optStr.c_str());
		}
	}

	if(dumpOptionsRead == true)
	{
		params.Dump();
	}
	
	return params;
}

bool OptFileParser::readRigParams(OptFileParams &params, string optStr, vector<string> lineTokens)
{
	bool optStrMatched = false;

	optStrMatched |= addDirName( "Rig.camera",          params.rigParams.cameraNames,         optStr, lineTokens);
	optStrMatched |= addCamPair( "Rig.camNeighPair",    params.rigParams.camNeighPairs,       optStr, lineTokens);
	optStrMatched |= readInt(    "Rig.startFrameIndex", params.rigParams.frameSeq.startIndex, optStr, lineTokens);
	optStrMatched |= readInt(    "Rig.endFrameIndex",   params.rigParams.frameSeq.endIndex,   optStr, lineTokens);
	optStrMatched |= readInt(    "Rig.targetCamIndex",  params.rigParams.targetCamIndex,      optStr, lineTokens);
	optStrMatched |= readBool(   "Rig.halfSizeCamMats", params.rigParams.halfSizeCamMats,     optStr, lineTokens);	
	optStrMatched |= readBool(   "Rig.useGeoCamDists",  params.rigParams.useGeoCamDists,      optStr, lineTokens);	
	optStrMatched |= readDirName("Rig.outDir",          params.rigParams.outDir,              optStr, lineTokens);

	optStrMatched |= addMV_Correspondances("Rig.mvc",   params.rigParams.userDefinedPlanes,   optStr, lineTokens);
	optStrMatched |= addTrack(             "Rig.track", params.rigParams.userDefinedTracks,   optStr, lineTokens);
	
	return optStrMatched;
}

bool OptFileParser::readRigCalibratorParams(OptFileParams &params, string optStr, vector<string> lineTokens)
{
	bool optStrMatched = false;

	optStrMatched |= readInt(    "RigCalibrator.framesToFind",     params.rigCalibratorParams.framesToFind,                optStr, lineTokens);
	optStrMatched |= readInt(    "RigCalibrator.startFrameIndex",  params.rigCalibratorParams.frameSeqToSearch.startIndex, optStr, lineTokens);
	optStrMatched |= readInt(    "RigCalibrator.endFrameIndex",    params.rigCalibratorParams.frameSeqToSearch.endIndex,   optStr, lineTokens);
	optStrMatched |= readInt(    "RigCalibrator.maxSharpenIters",  params.rigCalibratorParams.maxSharpenIters,             optStr, lineTokens);
	optStrMatched |= readBool(   "RigCalibrator.useNoahCalib",     params.rigCalibratorParams.useNoahCalib,                optStr, lineTokens);
	optStrMatched |= readBool(   "RigCalibrator.useLarryCalib",    params.rigCalibratorParams.useLarryCalib,               optStr, lineTokens);
	optStrMatched |= readString( "RigCalibrator.calibFN",          params.rigCalibratorParams.calibFN,                     optStr, lineTokens);

	optStrMatched |= readInt(    "RigCalibrator.minViewsForScenePoint", params.rigCalibratorParams.minViewsForScenePoint,  optStr, lineTokens);
	optStrMatched |= readDouble(  "RigCalibrator.minX",                 params.rigCalibratorParams.pcLowerBound[0],        optStr, lineTokens);
	optStrMatched |= readDouble(  "RigCalibrator.maxX",                 params.rigCalibratorParams.pcUpperBound[0],        optStr, lineTokens);
	optStrMatched |= readDouble(  "RigCalibrator.minY",                 params.rigCalibratorParams.pcLowerBound[1],        optStr, lineTokens);
	optStrMatched |= readDouble(  "RigCalibrator.maxY",                 params.rigCalibratorParams.pcUpperBound[1],        optStr, lineTokens);
	optStrMatched |= readDouble(  "RigCalibrator.minZ",                 params.rigCalibratorParams.pcLowerBound[2],        optStr, lineTokens);
	optStrMatched |= readDouble(  "RigCalibrator.maxZ",                 params.rigCalibratorParams.pcUpperBound[2],        optStr, lineTokens);

	return optStrMatched;
}

bool OptFileParser::readStereoParams(OptFileParams &params, string optStr, vector<string> lineTokens)
{
	bool optStrMatched = false;

	optStrMatched |= readFloat( "Stereo.noiseVar",                      params.stereoParams.noiseVar,                      optStr, lineTokens);
	optStrMatched |= readFloat( "Stereo.dataCostScale",                 params.stereoParams.dataCostScale,                 optStr, lineTokens);
	optStrMatched |= readFloat( "Stereo.dataCostExp",                   params.stereoParams.dataCostExp,                   optStr, lineTokens);
	optStrMatched |= readBool(  "Stereo.processInMem",                  params.stereoParams.processInMem,                  optStr, lineTokens);
	optStrMatched |= readBool(  "Stereo.onlyProcessPhotos",             params.stereoParams.onlyProcessPhotos,             optStr, lineTokens);
	optStrMatched |= readBool(  "Stereo.onlyUseScenePlanes",            params.stereoParams.onlyUseScenePlanes,            optStr, lineTokens);
	optStrMatched |= readInt(   "Stereo.bpIters",                       params.stereoParams.bpIters,                       optStr, lineTokens);
	optStrMatched |= readFloat( "Stereo.pixelDistBetweenFrontoPlanes",  params.stereoParams.pixelDistBetweenFrontoPlanes,  optStr, lineTokens);
	optStrMatched |= readInt(   "Stereo.spatialKernelSize",             params.stereoParams.spatialKernelSize,             optStr, lineTokens);

	optStrMatched |= readDouble("Stereo.minX",                          params.stereoParams.pcLowerBound[0],               optStr, lineTokens);
	optStrMatched |= readDouble("Stereo.maxX",                          params.stereoParams.pcUpperBound[0],               optStr, lineTokens);
	optStrMatched |= readDouble("Stereo.minY",                          params.stereoParams.pcLowerBound[1],               optStr, lineTokens);
	optStrMatched |= readDouble("Stereo.maxY",                          params.stereoParams.pcUpperBound[1],               optStr, lineTokens);
	optStrMatched |= readDouble("Stereo.minZ",                          params.stereoParams.pcLowerBound[2],               optStr, lineTokens);
	optStrMatched |= readDouble("Stereo.maxZ",                          params.stereoParams.pcUpperBound[2],               optStr, lineTokens);
	
	return optStrMatched;
}


bool OptFileParser::readStereoReconstructionParams(OptFileParams &params, string optStr, vector<string> lineTokens)
{
	bool optStrMatched = false;

	optStrMatched |= readInt(   "SR.iFrame",                  params.srParams.iFrame,                  optStr, lineTokens);
	optStrMatched |= readString("SR.targetCamID",             params.srParams.targetCamID,             optStr, lineTokens);
	optStrMatched |= addString( "SR.srsCamID",                params.srParams.srsCamIDs,               optStr, lineTokens);
	optStrMatched |= addFloat(  "SR.srsCamScale",             params.srParams.srsCamScales,            optStr, lineTokens);
	optStrMatched |= addBool(   "SR.srsCamFillerOnly",        params.srParams.srsCamFillerOnly,        optStr, lineTokens);
	optStrMatched |= readInt(   "SR.depthMatchThres",         params.srParams.depthMatchThres,         optStr, lineTokens);
	optStrMatched |= readInt(   "SR.iFrame",                  params.srParams.iFrame,                  optStr, lineTokens);	
	optStrMatched |= readInt(   "SR.depthDisConMaskThres",    params.srParams.depthDisConMaskThres,    optStr, lineTokens);
	optStrMatched |= readInt(   "SR.depthDisConMaskErWidth",  params.srParams.depthDisConMaskErWidth,  optStr, lineTokens);
	optStrMatched |= readFloat( "SR.targetScale",             params.srParams.targetScale,             optStr, lineTokens);
	optStrMatched |= readDouble("SR.targetCrop.x",            params.srParams.targetCropOrigin[0],     optStr, lineTokens);
	optStrMatched |= readDouble("SR.targetCrop.y",            params.srParams.targetCropOrigin[1],     optStr, lineTokens);
	optStrMatched |= readDouble("SR.targetCrop.w",            params.srParams.targetCropExt[0],        optStr, lineTokens);
	optStrMatched |= readDouble("SR.targetCrop.h",            params.srParams.targetCropExt[1],        optStr, lineTokens);
	optStrMatched |= readBool(  "SR.reconstructUsingAnalogy", params.srParams.reconstructUsingAnalogy, optStr, lineTokens);
	optStrMatched |= readBool(  "SR.useAnalogyMask",          params.srParams.useAnalogyMask,          optStr, lineTokens);
	

	optStrMatched |= readFloat(  "SR.PB.dataCostScale",    params.srParams.pbParams.dataCostScale, optStr, lineTokens);
	optStrMatched |= readBool(   "SR.PB.useGradCosts",     params.srParams.pbParams.useGradCosts,  optStr, lineTokens);
	
	
	optStrMatched |= readBool(      "SR.BS.simpleHoleBlending",  params.srParams.bsParams.simpleHoleBlending,  optStr, lineTokens);
	optStrMatched |= readInt(       "SR.BS.maxIters",            params.srParams.bsParams.maxIters,            optStr, lineTokens);
	optStrMatched |= readBool(      "SR.BS.fillInHoles",         params.srParams.bsParams.fillInHoles,         optStr, lineTokens);
	optStrMatched |= readFloat(     "SR.BS.highGradThres",       params.srParams.bsParams.highGradThres,       optStr, lineTokens);
	optStrMatched |= readBool(      "SR.BS.ignoreHighGradLinks", params.srParams.bsParams.ignoreHighGradLinks, optStr, lineTokens);
	optStrMatched |= readBool(      "SR.BS.useStrongBlending",   params.srParams.bsParams.useStrongBlending,   optStr, lineTokens);
	optStrMatched |= readImgFilter( "SR.BS.holesFilter",         params.srParams.bsParams.holesFilter,         optStr, lineTokens);

	return optStrMatched;
}

bool OptFileParser::readSS_SetupParams(OptFileParams &params, string optStr, vector<string> lineTokens)
{
	bool optStrMatched = false;

	optStrMatched |= readDirName("SS_Setup.targetDir",             params.ssSetupParams.targetDir,             optStr, lineTokens);
	optStrMatched |= readInt(    "SS_Setup.neighSpacing",          params.ssSetupParams.neighSpacing,          optStr, lineTokens);
	optStrMatched |= readString( "SS_Setup.noahBundleFN",          params.ssSetupParams.noahBundleFN,          optStr, lineTokens);
	optStrMatched |= readBool(   "SS_Setup.drewMode",              params.ssSetupParams.drewMode,              optStr, lineTokens);

	return optStrMatched;
}

bool OptFileParser::readSegmenterParams(OptFileParams &params, string optStr, vector<string> lineTokens)
{
	bool optStrMatched = false;

	optStrMatched |= readInt(  "Segmenter.smoothingLevel", params.stereoParams.segmenterParams.smoothingLevel, optStr, lineTokens);
	optStrMatched |= readInt(  "Segmenter.spatialRadius",  params.stereoParams.segmenterParams.spatialRadius,  optStr, lineTokens);
	optStrMatched |= readFloat("Segmenter.colorRadius",    params.stereoParams.segmenterParams.colorRadius,    optStr, lineTokens);

	optStrMatched |= readInt("Segmenter.minSegSize",     params.stereoParams.segmenterParams.segBreakerParams.minSegSize,    optStr, lineTokens);
	optStrMatched |= readInt("Segmenter.maxSegSize",     params.stereoParams.segmenterParams.segBreakerParams.maxSegSize,    optStr, lineTokens);
	optStrMatched |= readInt("Segmenter.breakGridSize",  params.stereoParams.segmenterParams.segBreakerParams.breakGridSize, optStr, lineTokens);

	int speedUpLevel;
	bool speedUpOptFound = readInt("Segmenter.speedUp", speedUpLevel, optStr, lineTokens);
	if(speedUpOptFound)
	{
		CHECK_RANGE(speedUpLevel, NO_SPEEDUP, HIGH_SPEEDUP);
		params.stereoParams.segmenterParams.speedUp = (SpeedUpLevel) speedUpLevel;
		optStrMatched = true;
	}

	return optStrMatched;
}



bool OptFileParser::readGenericParams(OptFileParams &params, string optStr, vector<string> lineTokens)
{
	bool optStrMatched = false;

	optStrMatched |= addString( "Generic.string1", params.genericParams.strList1,    optStr, lineTokens);
	optStrMatched |= addString( "Generic.string2", params.genericParams.strList2,    optStr, lineTokens);
	optStrMatched |= addInt(    "Generic.int1",    params.genericParams.intList1,    optStr, lineTokens);
	optStrMatched |= addInt(    "Generic.int2",    params.genericParams.intList2,    optStr, lineTokens);
	optStrMatched |= addFloat(  "Generic.float",  params.genericParams.floatList,    optStr, lineTokens);

	return optStrMatched;
}

bool OptFileParser::readFeaMatcherParams(OptFileParams &params, string optStr, vector<string> lineTokens)
{
	bool optStrMatched = false;

	optStrMatched |= readFloat("FeatureMatcher.firstToSecondRatio", params.feaMatcherParams.firstToSecondRatio, optStr, lineTokens);
	optStrMatched |= readFloat("FeatureMatcher.motionErrThres",     params.feaMatcherParams.motionErrThres,     optStr, lineTokens);

	return optStrMatched;
}

bool OptFileParser::readMFM_Params(OptFileParams &params, string optStr, vector<string> lineTokens)
{
	bool optStrMatched = false;

	optStrMatched |= readFloat("MFM.inlierThres", params.mfmParams.inlierThres, optStr, lineTokens);
	optStrMatched |= readInt(  "MFM.ransacIter",  params.mfmParams.ransacIter,  optStr, lineTokens);
	optStrMatched |= readInt(  "MFM.minInliers",  params.mfmParams.minInliers,  optStr, lineTokens);

	return optStrMatched;
}


bool OptFileParser::readPC_SegmenterParams(OptFileParams &params, string optStr, vector<string> lineTokens)
{
	bool optStrMatched = false;

	optStrMatched |= readFloat("PCS.ransacThresMult",  params.stereoParams.pcsParams.ransacThresMult,          optStr, lineTokens);
	optStrMatched |= readUint( "PCS.maxPlanesInScene", params.stereoParams.pcsParams.maxPlanesInScene,         optStr, lineTokens);
	optStrMatched |= readFloat("PCS.ransacThres",      params.stereoParams.pcsParams.ransacParams.ransacThres, optStr, lineTokens);
	optStrMatched |= readInt(  "PCS.minInliers",       params.stereoParams.pcsParams.ransacParams.minInliers,  optStr, lineTokens);
	optStrMatched |= readInt(  "PCS.ransacIter",       params.stereoParams.pcsParams.ransacParams.ransacIter,  optStr, lineTokens);

	return optStrMatched;
}

bool OptFileParser::readVideoSolverParams(OptFileParams &params, string optStr, vector<string> lineTokens)
{
	bool optStrMatched = false;

	optStrMatched |= readBool(  "VideoSolver.useFlow",                 params.vsParams.useFlow,                 optStr, lineTokens);
	optStrMatched |= readBool(  "VideoSolver.ccUsingQuery",            params.vsParams.ccUsingQuery,            optStr, lineTokens);	
	optStrMatched |= readBool(  "VideoSolver.ccTemporalImages",        params.vsParams.ccTemporalImages,        optStr, lineTokens);
	optStrMatched |= readBool(  "VideoSolver.useFlowErr",              params.vsParams.useFlowErr,              optStr, lineTokens);
	optStrMatched |= readFloat( "VideoSolver.errPow",                  params.vsParams.errPow,                  optStr, lineTokens);	
	optStrMatched |= readInt(   "VideoSolver.maxIters",                params.vsParams.maxIters,                optStr, lineTokens);
	optStrMatched |= readFloat( "VideoSolver.temporalBlendFrac",       params.vsParams.temporalBlendFrac,       optStr, lineTokens);
	optStrMatched |= readBool(  "VideoSolver.boostTempGradsUsingMask", params.vsParams.boostTempGradsUsingMask, optStr, lineTokens);
	optStrMatched |= readBool(  "VideoSolver.useNullTemporalGrads",    params.vsParams.useNullTemporalGrads,    optStr, lineTokens);
	optStrMatched |= readInt(   "VideoSolver.videoSolverFrameLimit",   params.vsParams.videoSolverFrameLimit,   optStr, lineTokens);
	optStrMatched |= readInt(   "VideoSolver.videoSolverMaxIters",     params.vsParams.videoSolverMaxIters,     optStr, lineTokens);
	optStrMatched |= readBool(  "VideoSolver.initUsingExistingSol",    params.vsParams.initUsingExistingSol,    optStr, lineTokens);
	optStrMatched |= readInt(   "VideoSolver.channel",                 params.vsParams.channel,                 optStr, lineTokens);
	optStrMatched |= readString("VideoSolver.outDir",                  params.vsParams.outDir,                  optStr, lineTokens);

	return optStrMatched;
}

