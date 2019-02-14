#include "VideoSolver.h"


string VideoSolver::getChannelResultName(int iFrame, int iChannel, const VideoSolverParams &params)
{
	return params.outDir + VideoSolver_CHANNEL_PREFIX[iChannel] +
		   Disk::GetBaseFileName(params.temporalImageNames[iFrame]) +
		   VideoSolver_CHANNEL_RESULT_EXT;
}

string VideoSolver::getChannelResultName(string temporalFileID, int iChannel, const VideoSolverParams &params)
{
	return params.outDir + VideoSolver_CHANNEL_PREFIX[iChannel] +
		   Disk::GetBaseFileName(temporalFileID) +
		   VideoSolver_CHANNEL_RESULT_EXT;
}


string VideoSolver::getResultName(int iFrame, const VideoSolverParams &params)
{
	return params.outDir + VideoSolver_RESULT_PREFIX + 
		   Disk::GetBaseFileName(params.temporalImageNames[iFrame]) +
		   VideoSolver_RESULT_EXT;
}


string VideoSolver::getResultCC_Name(int iFrame, const VideoSolverParams &params)
{
	return GetResultCC_Name(params.temporalImageNames[iFrame], params);
}

string VideoSolver::GetResultCC_Name(string temporalFileID, const VideoSolverParams &params)
{
	return params.outDir + VideoSolver_RESULT_CC_PREFIX + 
		   Disk::GetBaseFileName(temporalFileID) +
		   VideoSolver_RESULT_CC_EXT;
}

string VideoSolver::GetResultFNF_Name(string temporalFileID, const VideoSolverParams &params)
{
	return params.outDir + VideoSolver_RESULT_FNF_PREFIX + 
		   Disk::GetBaseFileName(temporalFileID) +
		   VideoSolver_RESULT_CC_EXT;
}


string VideoSolver::getVideoStatsFN(const VideoSolverParams &params)
{
	return params.outDir + "videoColorStats.txt";
}