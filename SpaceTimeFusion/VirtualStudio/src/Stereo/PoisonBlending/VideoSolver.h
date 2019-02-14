#ifndef __VIDEO_SOLVER_H__
#define __VIDEO_SOLVER_H__

#pragma warning(disable: 4512)

#include "Image.h"
#include "ImageProcessing.h"
#include "RegistrationDefs.h"
#include "SparseSolver.h"
#include <map>
#include "Disk.h"

const string VideoSolver_CHANNEL_PREFIX[]   = {"red-", "green-", "blue-"};
const string VideoSolver_RESULT_PREFIX      = "video-";
const string VideoSolver_RESULT_CC_PREFIX   = "cc-video-";
const string VideoSolver_RESULT_FNF_PREFIX  = "fnf-video-";

const string VideoSolver_CHANNEL_RESULT_EXT = ".raw";
const string VideoSolver_RESULT_EXT         = ".raw";
const string VideoSolver_RESULT_CC_EXT      = ".tif";

class VideoSolver
{	
	typedef enum
	{
		NOT_VAR     = -2,
		CONSTRAINED = -1,
	} NodeType;

	typedef std::map<uint, float> VarCoefList;
	typedef std::pair<uint, float> VarCoef;
	typedef std::pair<VarCoefList::iterator, bool> InsertResult;

	CShape imgShape;
	CShape maskShape;
	CShape flowShape;
	int frameCount;
	int varsPerFrame;	
	
	CFloatImage prevTemporalFrame;
	CFloatImage currTemporalFrame;
	CFloatImage nextTemporalFrame;
		
	CFloatImage prevSpatialFrame;
	CFloatImage currSpatialFrame;
	CFloatImage nextSpatialFrame;

	CFloatImage startConstraintFrame;
	CFloatImage startConstraintFrameSol;

	CFloatImage endConstraintFrame;
	CFloatImage endConstraintFrameSol;

	CFloatImage currToPrevFlow;
	CFloatImage currToNextFlow;
	CFloatImage nextToCurrFlow;
	CFloatImage prevToCurrFlow;

	CFloatImage currSolInitFrame;
    
	CFloatImage frameRHS;

	static const bool SymmetricSolver = true;

	vector<float> constrainedVoxelColor; 

	CVector3 spatialVideoLM;
	CVector3 spatialVideoLD;
	CVector3 temporalVideoLM;
	CVector3 temporalVideoLD;

public:
	typedef struct _BilateralParams
	{
		float spaceD;
		float sigD;
		int winHalfWidth;

		static _BilateralParams Default()
		{
			_BilateralParams params;
			params.spaceD = 8;
			params.sigD = 0.05f;
			params.winHalfWidth = 15;
			return params;
		}

	} BilateralParams;

	typedef struct _CrossBilateralParams
	{
        BilateralParams detailParams;
		BilateralParams flashNoFlashParams;
		static _CrossBilateralParams Default()
		{
			_CrossBilateralParams params;
			params.detailParams       = BilateralParams::Default();
			params.flashNoFlashParams = BilateralParams::Default();
			return params;
		}
	} CrossBilateralParams;

	typedef struct _VideoSolverParams
	{
		vector<string> spatialImageNames;
		vector<string> temporalImageNames;
		vector<string> frameMaskNames;
		string outDir;
		double accuracy;
		int    maxIters;
		bool   colorCorrect;

		bool   useStartConstraintFrame;
		string startConstraintFrame;
		string startConstraintMask;

		bool   useEndConstraintFrame;
		string endConstraintFrame;
		string endConstraintMask;

		bool   initUsingExistingSol;

		int    channel;
		float  errPow;

		float  temporalBlendFrac;

		bool  useFlow;
		bool  useFlowErr;
		bool  useNullTemporalGrads;

		bool ccTemporalImages;

		bool  ccUsingQuery;

		float spatialImgScale;
		float temporalImgScale;

		CrossBilateralParams cbfParams;

		int videoSolverFrameLimit;
		int videoSolverMaxIters;

		bool boostTempGradsUsingMask;
 
		static _VideoSolverParams Default()
		{
			_VideoSolverParams params;
			params.accuracy                = 1e-7;
			params.maxIters                = 50;	
			params.colorCorrect            = false;
			params.useStartConstraintFrame = false;
			params.useEndConstraintFrame   = false;
			params.initUsingExistingSol    = false;
			params.useFlow                 = true;
			params.spatialImgScale         = 1.0f;
			params.temporalImgScale        = 1.0f;
			params.temporalBlendFrac       = 0.9f;
			params.channel                 = -1;
			params.useFlowErr              = true; 
			params.useNullTemporalGrads    = false; 
			params.ccUsingQuery            = false;
			params.ccTemporalImages        = true;
			params.boostTempGradsUsingMask = false;
			params.errPow                  = 60;			
			params.cbfParams               = CrossBilateralParams::Default();
			params.videoSolverFrameLimit   = 20;
			params.videoSolverMaxIters     = 1;
			params.outDir                  = "./";
			return params;
		}

		void Dump(const string prefix)
		{
			printf("%sVideo Solver Options:\n", prefix.c_str());
			if(useStartConstraintFrame == false)
				printf("%s  useStartConstraintFrame = false\n", prefix.c_str());
			else
			{
				printf("%s  useStartConstraintFrame = true\n", prefix.c_str());
				printf("%s  startConstraintFrame    = %s\n",   prefix.c_str(), startConstraintFrame.c_str());
			}

			if(useEndConstraintFrame == false)
				printf("%s  useEndConstraintFrame  = false\n", prefix.c_str());
			else
			{
				printf("%s  useEndConstraintFrame  = true\n", prefix.c_str());
				printf("%s  endConstraintFrame     = %s\n",   prefix.c_str(), endConstraintFrame.c_str());
			}

			if(useNullTemporalGrads == false)
				printf("%s  useNullTemporalGrads = false\n", prefix.c_str());
			else
				printf("%s  useNullTemporalGrads = true\n", prefix.c_str());

			if(initUsingExistingSol == true)
				printf("%s  initUsingExistingSol  = true\n", prefix.c_str());
			else
				printf("%s  initUsingExistingSol  = false\n", prefix.c_str());

			if(boostTempGradsUsingMask == true)
				printf("%s  boostTempGradsUsingMask  = true\n", prefix.c_str());
			else
				printf("%s  boostTempGradsUsingMask  = false\n", prefix.c_str());	

			if(ccTemporalImages == true)
				printf("%s  ccTemporalImages  = true\n", prefix.c_str());
			else
				printf("%s  ccTemporalImages  = false\n", prefix.c_str());

			printf("%s  accuracy                = %lf\n", prefix.c_str(), accuracy);
			printf("%s  maxIters                = %i\n",  prefix.c_str(), maxIters);		
			printf("%s  temporalBlendFrac       = %f\n",  prefix.c_str(), temporalBlendFrac);
			printf("%s  outDir                  = %s\n",  prefix.c_str(), outDir.c_str());
			printf("%s  channel                 = %i\n",  prefix.c_str(), channel);
			if(colorCorrect == true)
			{
				printf("%s  colorCorrect            = true\n", prefix.c_str());
				if(ccUsingQuery == true)				
					printf("%s  ccUsingQuery            = true\n", prefix.c_str());
				else
					printf("%s  ccUsingQuery            = false\n", prefix.c_str());
			}
			else
			{
				printf("%s  colorCorrect            = false\n", prefix.c_str());
			}
			if(useFlow == true)
			{
				printf("%s  useFlow                 = true\n", prefix.c_str());
				
				if(useFlow == true)
					printf("%s  useFlowErr              = true\n", prefix.c_str());
				else
					printf("%s  useFlowErr              = false\n", prefix.c_str());

				printf("%s  errPow                 = %f\n", prefix.c_str(), errPow);
			}
			else
			{
				printf("%s  useFlow                 = false\n", prefix.c_str());
			}
			printf("\n");

			printf("%s  spatialImgScale = %f\n", prefix.c_str(), spatialImgScale);
			for(uint i = 0; i < spatialImageNames.size(); i++)
			{
				printf("%s  Spatial  frame: %s\n", prefix.c_str(), spatialImageNames[i].c_str());
			}
			printf("\n");

			printf("%s  temporalImgScale = %f\n", prefix.c_str(), temporalImgScale);
			for(uint i = 0; i < temporalImageNames.size(); i++)
			{
				printf("%s  Temporal frame: %s\n", prefix.c_str(), temporalImageNames[i].c_str());
			}

			printf("%s  videoSolverFrameLimit = %i\n", prefix.c_str(), videoSolverFrameLimit);
			printf("%s  videoSolverMaxIters   = %i\n", prefix.c_str(), videoSolverMaxIters);			
		}

	} VideoSolverParams;

	void Solve(VideoSolverParams params);

	void CombineResults(VideoSolverParams params);

private:
	void loadTemporalFrame(CFloatImage &temporalFrame, string temporalFrameFN, const VideoSolverParams &params);

	void updateErrMask(CFloatImage &flowField, string fieldID);

	void boostTemporalGrads(CFloatImage &flowField, string maskFileID);

	void combineResults(const VideoSolverParams &params);

	void collectSpatialNeighbours(VarCoefList *frameNeighbours,
								  const VideoSolverParams &params);

	void collectTemporalNeighbours(VarCoefList *frameNeighbours, 
								   CFloatImage &outTemporalFlow, bool collectOutNeighbours,
								   CFloatImage &inTemporalFlow,  bool collectInNeighbours,
								   const VideoSolverParams &params);

	

	void initGlobals(const VideoSolverParams &params, bool readVidStats = true);

	void loadConstraints(int channel, const VideoSolverParams &params);

	void loadCurrFrameContext(int iFrame, int iChannel, const VideoSolverParams &params);

	void defineFrameEqs(int iChannel, int iFrame, 
						VarCoefList *prevFrameNeighbours,
						VarCoefList *currFrameNeighbours,
						VarCoefList *nextFrameNeighbours,						
						SparseSolver &solver, const VideoSolverParams &params);

	int getNumOfVars()
	{
		return (this->imgShape.width * this->imgShape.height * this->frameCount);
	}

	int getNodeVar(int iFrame, uint nodeAddr)
	{
		return ((iFrame * this->varsPerFrame) + nodeAddr);
	}

	void getVoxelAddr(int nodeVar, int &iFrame, uint &nodeAddr)
	{
		iFrame   = (nodeVar / this->varsPerFrame);
		nodeAddr = (nodeVar % this->varsPerFrame);
	}

	void writeResults(int channel, SparseSolver &solver, const VideoSolverParams &params);

public:  //VideoSolver-FileNames.cpp
	static string GetResultCC_Name(string temporalFileID, const VideoSolverParams &params);	
	static string GetResultFNF_Name(string temporalFileID, const VideoSolverParams &params);	


private: //VideoSolver-FileNames.cpp
	string getChannelResultName(int iFrame, int iChannel, const VideoSolverParams &params);
	string getChannelResultName(string temporalFileID, int iChannel, const VideoSolverParams &params);
	string getResultName(int iFrame, const VideoSolverParams &params);
	string getResultCC_Name(int iFrame, const VideoSolverParams &params);	
	string getVideoStatsFN(const VideoSolverParams &params);


public: //VideoSolver-Color.cpp
	void ColorCorrect(VideoSolverParams params);

	void ComputeVideoStats(VideoSolverParams params);

private: //VideoSolver-Color.cpp
	void computeVideoStats(const vector<string> &videoFrameFNs, float videoScale,
						   CVector3 &videoLabMean, CVector3 &videoLavDev);

	void writeVideoStats(const VideoSolverParams &params);
	void readVideoStats(const VideoSolverParams &params);

	void updateVideoLabMean(CFloatImage &rgbFrame, CVector3 &meanVec);
	void updateVideoLabVarSum(CFloatImage &rgbFrame, CVector3 &meanVec, CVector3 &varSum);

	void colorCorrectSol(CFloatImage &videoFrameRGB, 
						 CVector3 &videoMeanVec,  CVector3 &videoStdDevVec,
						 CVector3 &targetMeanVec, CVector3 &targetStdDevVec);

	void colorCorrectSol(CVector3 &solMeanVec,    CVector3 &solStdDevVec,
						 CVector3 &targetMeanVec, CVector3 &targetStdDevVec,
		  			     const VideoSolverParams &params);

	void colorCorrectResult(const VideoSolverParams &params);


};

#endif
