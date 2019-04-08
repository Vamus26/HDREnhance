#include "VideoSolver.h"
void VideoSolver::ComputeVideoStats(VideoSolverParams params)
{
	initGlobals(params, false);

	for(int iChannel = 0; iChannel < 3; iChannel++)
	{
		this->spatialVideoLM[iChannel]  = this->spatialVideoLD[iChannel]  = 0.0;
		this->temporalVideoLM[iChannel] = this->temporalVideoLD[iChannel] = 0.0;
	}

	vector<string> spatialVideoFrameFNs(this->frameCount);
	vector<string> temporalVideoFrameFNs(this->frameCount);
	for(int iFrame = 0; iFrame < this->frameCount; iFrame++)
	{		
		spatialVideoFrameFNs[iFrame]  = params.spatialImageNames[iFrame]  + ".tif";
		temporalVideoFrameFNs[iFrame] = params.temporalImageNames[iFrame] + ".tif";
	}

	printf("Computing spatial video stats...\n");
	computeVideoStats(spatialVideoFrameFNs, params.spatialImgScale,
			          spatialVideoLM, spatialVideoLD);

	printf("Computing temporal video stats...\n");
	computeVideoStats(temporalVideoFrameFNs, params.temporalImgScale,
			          temporalVideoLM, temporalVideoLD);

	writeVideoStats(params);
}

void VideoSolver::ColorCorrect(VideoSolverParams params)
{
	initGlobals(params);

	CVector3 solVideoLM( 0.0, 0.0, 0.0 );
	CVector3 solVideoLD( 0.0, 0.0, 0.0 );

	vector<string> solVideoFrameFNs(this->frameCount);
	for(int iFrame = 0; iFrame < this->frameCount; iFrame++)
	{		
		solVideoFrameFNs[iFrame] = getResultName(iFrame, params);	
	}

	printf("Computing solution video stats...\n");
	computeVideoStats(solVideoFrameFNs, 1.0f,
			          solVideoLM, solVideoLD);


	CVector3 targetVideoLM( 0.0, 0.0, 0.0 );
	CVector3 targetVideoLD( 0.0, 0.0, 0.0 );

	if(params.ccUsingQuery)
	{
		targetVideoLM = this->temporalVideoLM;
		targetVideoLD = this->temporalVideoLD;
	}
	else
	{
		targetVideoLM = this->spatialVideoLM;
		targetVideoLD = this->spatialVideoLD;
	}

	colorCorrectSol(solVideoLM, solVideoLD,
					targetVideoLM, targetVideoLD, params);

	//targetVideoLM[0] = solVideoLD[0];
	//colorCorrectSol(solVideoLM, solVideoLD,
	//				targetVideoLM, targetVideoLD, params);
}


void VideoSolver::computeVideoStats(const vector<string> &videoFrameFNs, float videoScale,
									CVector3 &videoLabMean, CVector3 &videoLavDev)
{
	ENSURE(videoFrameFNs.size() > 0);
	CFloatImage videoFrame(this->imgShape);

	for(int i = 0; i <= 1; i++)
	{
		for(uint iFrame = 0; iFrame < videoFrameFNs.size(); iFrame++)
		{		
			ImageIO::ReadFile(videoFrame,
							  videoFrameFNs[iFrame],
							  videoScale);

			if(i == 0)
			{
				updateVideoLabMean(videoFrame, videoLabMean);
  		    }
			else
			{
				ENSURE(i == 1);
				updateVideoLabVarSum(videoFrame, videoLabMean, videoLavDev);
			}			
			printf(".");
			fflush(stdout);
		}
		printf("\n");
	}

	int channelNodeCount = this->imgShape.width * this->imgShape.height * this->frameCount;
	for(int iChannel = 0; iChannel < 3; iChannel++)
	{
		videoLavDev[iChannel] = sqrt(videoLavDev[iChannel] / channelNodeCount);
	}
}

void VideoSolver::colorCorrectSol(CFloatImage &videoFrameRGB, 
								  CVector3 &videoMeanVec,  CVector3 &videoStdDevVec,
								  CVector3 &targetMeanVec, CVector3 &targetStdDevVec)
{
	CShape labImgShape = videoFrameRGB.Shape();
	labImgShape.nBands = 3;
	CFloatImage videoFrameLAB(labImgShape);
	ColorHelper::RGBtoLAB(videoFrameLAB, videoFrameRGB);

	for(int channel = 0; channel < labImgShape.nBands; channel++)
	{
		ENSURE(videoStdDevVec[channel] != 0);
		double stdDevRatio = targetStdDevVec[channel] / videoStdDevVec[channel];
		double newMean     = targetMeanVec[channel];
		double oldMean     = videoMeanVec[channel];			
		uint pixelAddr     = 0;
		for(int imageY = 0; imageY < this->imgShape.height; imageY++)
		{
			for(int imageX = 0; imageX < this->imgShape.width; imageX++, pixelAddr += labImgShape.nBands)
			{
				double oldPixelVal = videoFrameLAB[pixelAddr + channel];
				videoFrameLAB[pixelAddr + channel] = (float) ((stdDevRatio * (oldPixelVal - oldMean)) + newMean);
			}
		}
	}
	ColorHelper::LABtoRGB(videoFrameRGB, videoFrameLAB);
}

void VideoSolver::colorCorrectSol(CVector3 &solMeanVec,    CVector3 &solStdDevVec,
								  CVector3 &targetMeanVec, CVector3 &targetStdDevVec,
		  						  const VideoSolverParams &params)
{
	for(int iLabChannel = 0; iLabChannel < 3; iLabChannel++)
	{
		ENSURE(solStdDevVec[iLabChannel] != 0.0);
	}

	printf("Color correction...\n");
	CFloatImage currFrameRGB(this->imgShape);
	for(int iFrame = 0; iFrame < this->frameCount; iFrame++)
	{
		printf(".");
		string inName = getResultName(iFrame, params);
		ImageIO::ReadFile(currFrameRGB, inName);
		ENSURE(currFrameRGB.Shape() == this->imgShape);

		colorCorrectSol(currFrameRGB, 
						solMeanVec, solStdDevVec,
						targetMeanVec, targetStdDevVec);

		string outName = getResultCC_Name(iFrame, params);
		ImageIO::WriteFile(currFrameRGB, outName);
		fflush(stdout);
	}
	printf("\n");
}


void VideoSolver::updateVideoLabMean(CFloatImage &rgbFrame, CVector3 &meanVec)
{	
	CShape labImgShape = rgbFrame.Shape();
	labImgShape.nBands = 3;
	CFloatImage currLabImg(labImgShape);
	ColorHelper::RGBtoLAB(currLabImg, rgbFrame);

	for(int iChannel = 0; iChannel < labImgShape.nBands; iChannel++)
	{
		float frameMean   = 0;
		float frameStdDev = 0;
		ImageProcessing::GetMeanAndDev(currLabImg, iChannel, frameMean, frameStdDev);
		meanVec[iChannel] += (((double) frameMean) / this->frameCount);
	}
}

void VideoSolver::updateVideoLabVarSum(CFloatImage &rgbFrame, CVector3 &meanVec, CVector3 &varSum)
{
	CShape labImgShape = rgbFrame.Shape();
	labImgShape.nBands = 3;
	CFloatImage currLabImg(labImgShape);
	ColorHelper::RGBtoLAB(currLabImg, rgbFrame);

	int sampleCount = labImgShape.width * labImgShape.height * labImgShape.nBands;
	for(int iSample = 0; iSample < sampleCount; iSample += labImgShape.nBands)
	{
		for(int iChannel = 0; iChannel < labImgShape.nBands; iChannel++)
		{
			double dev = currLabImg[iSample + iChannel] - meanVec[iChannel];
			varSum[iChannel] += (dev * dev);
		}
	}
}

void VideoSolver::writeVideoStats(const VideoSolverParams &params)
{
	string vidStatsFN = getVideoStatsFN(params);
	ofstream outStream(vidStatsFN.c_str());
	ENSURE(outStream.is_open());

	for(int iChannel = 0; iChannel < 3; iChannel++)
	{
		outStream << this->spatialVideoLM[iChannel]  << " " << this->spatialVideoLD[iChannel]  << "\n";
		outStream << this->temporalVideoLM[iChannel] << " " << this->temporalVideoLD[iChannel] << "\n";
	}

	outStream.close();
}

void VideoSolver::readVideoStats(const VideoSolverParams &params)
{
	string vidStatsFN = getVideoStatsFN(params);
	ifstream inStream(vidStatsFN.c_str());
	ENSURE(inStream.is_open());

	for(int iChannel = 0; iChannel < 3; iChannel++)
	{
		this->spatialVideoLM[iChannel]  = this->spatialVideoLD[iChannel]  = FLT_MAX;
		this->temporalVideoLM[iChannel] = this->temporalVideoLD[iChannel] = FLT_MAX;
	}

	for(int iChannel = 0; iChannel < 3; iChannel++)
	{
		inStream >> this->spatialVideoLM[iChannel];
		inStream >> this->spatialVideoLD[iChannel];
		inStream >> this->temporalVideoLM[iChannel];
		inStream >> this->temporalVideoLD[iChannel];
	}

	for(int iChannel = 0; iChannel < 3; iChannel++)
	{
		ENSURE(this->spatialVideoLM[iChannel]  != FLT_MAX);
		ENSURE(this->spatialVideoLD[iChannel]  != FLT_MAX);
		ENSURE(this->temporalVideoLM[iChannel] != FLT_MAX);
		ENSURE(this->temporalVideoLD[iChannel] != FLT_MAX);
	}

	inStream.close();
}