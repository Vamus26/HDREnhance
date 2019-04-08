#include "VideoSolver.h"
#include "Timer.h"
#include "BlendSolver.h"
#include "ColorHelper.h"
#include "FlowHelper.h"
#include "Disk.h"

#pragma warning(disable : 4127)

void VideoSolver::loadTemporalFrame(CFloatImage &temporalFrame, string temporalFrameFN, const VideoSolverParams &params)
{
	ImageIO::ReadFile(temporalFrame,
					  temporalFrameFN,
					  params.temporalImgScale);

	if(params.ccTemporalImages)
	{
		colorCorrectSol(temporalFrame, 
						this->temporalVideoLM, this->temporalVideoLD,
						this->spatialVideoLM, this->spatialVideoLD);
	}
}

void VideoSolver::initGlobals(const VideoSolverParams &params, bool readVidStats)
{
	ENSURE((params.colorCorrect == false) || 
		   ((params.useStartConstraintFrame == false) && (params.useEndConstraintFrame == false)));
	ENSURE(params.temporalImageNames.size() == params.spatialImageNames.size());
	ENSURE(params.temporalImageNames.size() > 0);

	if(readVidStats)
		readVideoStats(params);
	else
	{
		for(int iChannel = 0; iChannel < 3; iChannel++)
		{
			this->spatialVideoLM[iChannel]  = this->spatialVideoLD[iChannel]  = 0.0;
			this->temporalVideoLM[iChannel] = this->temporalVideoLD[iChannel] = 0.0;
		}
	}

	this->frameCount = (int) params.temporalImageNames.size();
	ENSURE(this->frameCount == (int) params.spatialImageNames.size());
	
	this->imgShape = ImageIO::GetImageShape(params.temporalImageNames[0] + ".tif",
											params.temporalImgScale);
	ENSURE(this->imgShape.nBands == 3);

	this->maskShape = CShape(this->imgShape.width, this->imgShape.height, 1);
	this->varsPerFrame = this->maskShape.width * this->maskShape.height;
	this->frameRHS.ReAllocate(this->maskShape);

	this->flowShape = CShape(this->imgShape.width, this->imgShape.height, 3);

	this->constrainedVoxelColor.resize(this->imgShape.nBands);	
}

void VideoSolver::boostTemporalGrads(CFloatImage &flowField, string maskFileID)
{
    CFloatImage maskImg;

	ImageIO::ReadFile(maskImg, maskFileID + ".tif");
	ENSURE(this->imgShape.SameIgnoringNBands(maskImg.Shape()));
	ENSURE(this->imgShape.SameIgnoringNBands(flowField.Shape()));
	ENSURE(flowField.Shape().nBands == 3);

	if(maskImg.Shape().nBands == 3)
		maskImg = ColorHelper::RGBtoLuminance(maskImg);
	else
	{
		INSIST(maskImg.Shape().nBands == 1);
	}

	CByteImage mask = ImageProcessing::FloatToByteImg(maskImg);

	uint nodeAddr = 0;
	uint flowAddr = 0;
	//bug
	//for(int y = 0; y < this->imgShape.height; y++)
	//for(int x = 0; x < this->imgShape.width;  x++, nodeAddr++, flowAddr += 3)
	//{
	//	//if(flowField[flowAddr + 2] != 0.0f)
	//	//	//flowField[flowAddr + 2] += (maskImg[nodeAddr] * 240.0f); //check
	//	//	flowField[flowAddr + 2] += (maskImg[nodeAddr] * 160.0f); //check

	//	if(mask[nodeAddr] == MASK_INVALID)
	//		flowField[flowAddr + 2] = 0.0f;
	//}



	REPORT("boosting temporal grads\n");
}

void VideoSolver::loadConstraints(int channel, const VideoSolverParams &params)
{
	if(params.useStartConstraintFrame == true)
	{
		loadTemporalFrame(this->startConstraintFrame, 
						  params.startConstraintFrame + ".tif", 
						  params);
		ENSURE(this->startConstraintFrame.Shape() == this->imgShape);
		if(params.useNullTemporalGrads == true)
		{
			printf("Using null temporal gradients\n");
			this->startConstraintFrame.ClearPixels();
		}

		string startConstraintFrameSolFN = getChannelResultName(params.startConstraintFrame, channel, params);
		ImageIO::ReadFile(this->startConstraintFrameSol, startConstraintFrameSolFN);
		ENSURE(this->startConstraintFrameSol.Shape() == this->maskShape);
	}

	if(params.useEndConstraintFrame == true)
	{
		loadTemporalFrame(this->endConstraintFrame, 
						  params.endConstraintFrame + ".tif", 
						  params);
		ENSURE(this->endConstraintFrame.Shape() == imgShape);
		if(params.useNullTemporalGrads == true)
		{
			printf("Using null temporal gradients\n");
			this->endConstraintFrame.ClearPixels();
		}

		string endConstraintFrameSolFN = getChannelResultName(params.endConstraintFrame, channel, params);
		ImageIO::ReadFile(this->endConstraintFrameSol, endConstraintFrameSolFN);
		ENSURE(this->endConstraintFrameSol.Shape() == this->maskShape);		
	}
}

void VideoSolver::loadCurrFrameContext(int iFrame, int iChannel, const VideoSolverParams &params)
{
	ENSURE(iFrame >= 0);
	ENSURE(iFrame < this->frameCount);

	CFloatImage nullImage;	

	if(iFrame > 0)
	{	
		this->prevTemporalFrame  = this->currTemporalFrame.Clone();
		this->currTemporalFrame  = this->nextTemporalFrame.Clone();
		this->prevSpatialFrame   = this->currSpatialFrame.Clone();
		this->currSpatialFrame   = this->nextSpatialFrame.Clone();
		if(params.useFlow == true)
		{
			this->currToPrevFlow = this->nextToCurrFlow.Clone();
			this->prevToCurrFlow = this->currToNextFlow.Clone();
		}		
	}
	else
	{
		this->prevTemporalFrame = nullImage;
		this->prevSpatialFrame  = nullImage;
		this->currToPrevFlow    = nullImage;
		this->prevToCurrFlow    = nullImage;
		if(params.useStartConstraintFrame == true)
		{
			if(params.useFlow == true)
			{
				FlowHelper::ReadFlowField(params.temporalImageNames[iFrame] + ".flow", 
										  this->currToPrevFlow, params.temporalImgScale);
				ENSURE(this->currToPrevFlow.Shape() == this->flowShape);
	
				FlowHelper::ReadFlowField(params.startConstraintFrame + ".revflow", 
										  this->prevToCurrFlow, params.temporalImgScale);
				ENSURE(this->prevToCurrFlow.Shape() == this->flowShape);

				if(params.boostTempGradsUsingMask)
				{
					boostTemporalGrads(this->prevToCurrFlow, params.startConstraintMask);
					boostTemporalGrads(this->currToPrevFlow, params.frameMaskNames[iFrame]);
				}
			}
		}

		ImageIO::ReadFile(this->currSpatialFrame, 
					      params.spatialImageNames[iFrame] + ".tif",
						  params.spatialImgScale);
		ENSURE(this->currSpatialFrame.Shape() == this->imgShape);

		loadTemporalFrame(this->currTemporalFrame, 
						  params.temporalImageNames[iFrame] + ".tif",
						  params);
		ENSURE(this->currTemporalFrame.Shape() == this->imgShape);

		for(int channel = 0; channel < this->imgShape.nBands; channel++)
		{
			this->constrainedVoxelColor[channel] = this->currSpatialFrame.Pixel(0, 0, channel);
		}
	}

	if(iFrame < this->frameCount - 1)
	{
		loadTemporalFrame(this->nextTemporalFrame, 
						  params.temporalImageNames[iFrame + 1] + ".tif",
						  params);
		ENSURE(this->nextTemporalFrame.Shape() == this->imgShape);

		ImageIO::ReadFile(this->nextSpatialFrame, 
						  params.spatialImageNames[iFrame + 1] + ".tif",
						  params.spatialImgScale);
		ENSURE(this->nextSpatialFrame.Shape() == this->imgShape);

		if(params.useFlow == true)
		{
			FlowHelper::ReadFlowField(params.temporalImageNames[iFrame] + ".revflow", 
									  this->currToNextFlow, params.temporalImgScale);
			ENSURE(this->currToNextFlow.Shape() == this->flowShape);

			FlowHelper::ReadFlowField(params.temporalImageNames[iFrame + 1] + ".flow", 
									  this->nextToCurrFlow, params.temporalImgScale);
			ENSURE(this->nextToCurrFlow.Shape() == this->flowShape);

			if(params.boostTempGradsUsingMask)
			{
				boostTemporalGrads(this->currToNextFlow, params.frameMaskNames[iFrame]);
				boostTemporalGrads(this->nextToCurrFlow, params.frameMaskNames[iFrame + 1]);
			}
		}
	}
	else
	{
		this->nextTemporalFrame = nullImage;
		this->nextSpatialFrame  = nullImage;
		this->currToNextFlow    = nullImage;
		this->nextToCurrFlow    = nullImage;

		if(params.useEndConstraintFrame == true)
		{
			if(params.useFlow == true)
			{
				FlowHelper::ReadFlowField(params.temporalImageNames[iFrame] + ".revflow", 
										  this->currToNextFlow, params.temporalImgScale);
				ENSURE(this->currToNextFlow.Shape() == this->flowShape);

				FlowHelper::ReadFlowField(params.endConstraintFrame + ".flow", 
										  this->nextToCurrFlow, params.temporalImgScale);
				ENSURE(this->nextToCurrFlow.Shape() == this->flowShape);

				if(params.boostTempGradsUsingMask)
				{
					boostTemporalGrads(this->currToNextFlow, params.frameMaskNames[iFrame]);
					boostTemporalGrads(this->nextToCurrFlow, params.endConstraintMask);
				}
			}
		}
	}
	
	if(params.useNullTemporalGrads == true)
	{
		this->prevTemporalFrame.ClearPixels();
		this->currTemporalFrame.ClearPixels();
		this->nextTemporalFrame.ClearPixels();
	}
	
	if(params.initUsingExistingSol)
	{
		string currSolInitFrameFN = getChannelResultName(iFrame, iChannel, params);
		ImageIO::ReadFile(this->currSolInitFrame, currSolInitFrameFN);
		ENSURE(this->currSolInitFrame.Shape() == this->maskShape);
	}
	else
	{
		this->currSolInitFrame.ReAllocate(this->maskShape);
		ImageProcessing::CopyChannel(this->currSolInitFrame, this->currSpatialFrame, 0, iChannel);
	}
}

void VideoSolver::collectSpatialNeighbours(VarCoefList *frameNeighbours,
										   const VideoSolverParams &params)
{
	const int   neighourCount       = 5;
	const int   dx[neighourCount]   = { 0,    -1,    0,    1,    0};
	const int   dy[neighourCount]   = {-1,     0,    0,    0,    1};
		  float coef[neighourCount] = {2.0f, 2.0f, 0.0f, 2.0f, 2.0f};

	ENSURE(params.temporalBlendFrac >= 0.0f);
	ENSURE(params.temporalBlendFrac <= 1.0f);
	for(int iNeighbour = 0; iNeighbour < neighourCount; iNeighbour++)
	{
		coef[iNeighbour] *= (1.0f - params.temporalBlendFrac);
	}

	uint nodeAddr  = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++)
		{
			VarCoefList &currNodeNeighbours = frameNeighbours[nodeAddr];
			for(int iNeighbour = 0; iNeighbour < neighourCount; iNeighbour++)
			{
				int neighbourX = x + dx[iNeighbour];
				int neighbourY = y + dy[iNeighbour];
				if(this->maskShape.InBounds(neighbourX, neighbourY) == true)
				{
					uint neighbourNodeAddr = this->maskShape.NodeIndex(neighbourX, neighbourY);
					VarCoef varCoefPair(neighbourNodeAddr, coef[iNeighbour]);
					InsertResult result = currNodeNeighbours.insert(varCoefPair);
					ENSURE(result.second == true);
				}
			}
		}
	}
}

/*
void VideoSolver::collectTemporalNeighbours(VarCoefList *frameNeighbours, 
										    CFloatImage &outTemporalFlow, bool collectOutNeighbours,
										    CFloatImage &inTemporalFlow,  bool collectInNeighbours,
											const VideoSolverParams &params)
{
	ENSURE((collectOutNeighbours == true) || (collectInNeighbours == true));
	ENSURE(params.temporalBlendFrac >= 0.0f);
	ENSURE(params.temporalBlendFrac <= 1.0f);
	float temporalCoef = 1.0f * params.temporalBlendFrac;

	if(params.useFlow == true)
	{
		ENSURE(outTemporalFlow.Shape() == this->flowShape);
		ENSURE(inTemporalFlow.Shape()  == this->flowShape);
	}

	uint nodeAddr  = 0;
	uint flowAddr  = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++, flowAddr += this->flowShape.nBands)
		{
			if(collectOutNeighbours == true)
			{
				int   u    = (params.useFlow == true) ?  NEAREST_INT(outTemporalFlow[flowAddr + 0]) : 0;
				int   v    = (params.useFlow == true) ?  NEAREST_INT(outTemporalFlow[flowAddr + 1]) : 0;
				float err  = ((params.useFlow == true) && (params.useFlowErr == true)) ?  outTemporalFlow[flowAddr + 2] : 1;

				int outX = x - u;				
				int outY = y - v;
				float coef = (temporalCoef * pow(err, params.errPow));
				if(this->maskShape.InBounds(outX, outY))
				{
					uint outAddr = this->maskShape.NodeIndex(outX, outY);
					VarCoef varCoefPair(outAddr, coef);
					InsertResult result = frameNeighbours[nodeAddr].insert(varCoefPair);
					if(result.second == false)
					{
						result.first->second += coef;
					}
				}
			}

			if(collectInNeighbours == true)
			{
				int   u    = (params.useFlow == true) ?  NEAREST_INT(inTemporalFlow[flowAddr + 0]) : 0;
				int   v    = (params.useFlow == true) ?  NEAREST_INT(inTemporalFlow[flowAddr + 1]) : 0;
				float err  = ((params.useFlow == true) && (params.useFlowErr == true)) ?  inTemporalFlow[flowAddr + 2] : 1;
		
				int inX = x - u;
				int inY = y - v;
				float coef = (temporalCoef * pow(err, params.errPow));
				if(this->maskShape.InBounds(inX, inY))
				{
					uint inAddr = this->maskShape.NodeIndex(inX, inY);
					VarCoef varCoefPair(nodeAddr, coef);
					InsertResult result = frameNeighbours[inAddr].insert(varCoefPair);
					if(result.second == false)
					{
						result.first->second += coef;
					}
				}
			}
		}
	}
}
//*/


///*
void VideoSolver::collectTemporalNeighbours(VarCoefList *frameNeighbours, 
										    CFloatImage &outTemporalFlow, bool collectOutNeighbours,
										    CFloatImage &inTemporalFlow,  bool collectInNeighbours,
											const VideoSolverParams &params)
{
	ENSURE((collectOutNeighbours == true) || (collectInNeighbours == true));
	ENSURE(params.temporalBlendFrac >= 0.0f);
	ENSURE(params.temporalBlendFrac <= 1.0f);
	float temporalCoef = 1.0f * params.temporalBlendFrac;

	CFloatImage outFlow;
	CFloatImage inFlow;
	ENSURE((params.useFlow == true) || (params.useFlowErr == false));
	if(params.useFlow)
	{
		ENSURE(outTemporalFlow.Shape() == this->flowShape);
		ENSURE(inTemporalFlow.Shape()  == this->flowShape);
		outFlow = outTemporalFlow.Clone();
		inFlow  = inTemporalFlow.Clone();
	}
	else
	{
		outFlow.ReAllocate(this->flowShape);
	    outFlow.ClearPixels();
		inFlow.ReAllocate(this->flowShape);
		inFlow.ClearPixels();
	}

	if(params.useFlowErr == false)
	{
		ImageProcessing::Operate(outFlow, outFlow, 1.0f, ImageProcessing::OP_SET, 2);
		ImageProcessing::Operate(inFlow,  inFlow,  1.0f, ImageProcessing::OP_SET, 2);
	}

	uint nodeAddr  = 0;
	uint flowAddr  = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	for(int x = 0; x < this->maskShape.width;  x++, nodeAddr++, flowAddr += this->flowShape.nBands)
	for(int flowType = 0; flowType <= 1; flowType++)
	{
		if(flowType == 0)
		{
			if (collectOutNeighbours == false) continue;
		}
		else
		{
			ENSURE(flowType == 1);
			if (collectInNeighbours == false) continue;
		}

		float u;
		float v;  
		float err;

		if(flowType == 0)
		{
			u    = outFlow[flowAddr + 0];
			v    = outFlow[flowAddr + 1];
			err  = outFlow[flowAddr + 2];
		}
		else
		{
			u    = inFlow[flowAddr + 0];
			v    = inFlow[flowAddr + 1];
			err  = inFlow[flowAddr + 2];
		}

		float targetX = x - u;
		float targetY = y - v;

		if(err < 1.0)
			err = pow(err, params.errPow);
		float coef = (temporalCoef * err);

		if(this->maskShape.InBounds(targetX, targetY))
		{
			int floorTargetX = (int) targetX;
			int roofTargetX  = (int) (targetX + 1.0f);

			int floorTargetY = (int) targetY;
			int roofTargetY  = (int) (targetY + 1.0f);

			float xRoofFrac  = targetX - floorTargetX;
		  	float yRoofFrac  = targetY - floorTargetY;
			float xFloorFrac = 1.0f - xRoofFrac;
			float yFloorFrac = 1.0f - yRoofFrac;

			for(int iNeigh = 0; iNeigh < 4; iNeigh++)
			{
				int neighX = INT_MAX, neighY = INT_MAX;
				float neighCoefFrac = 0.0f;

				switch(iNeigh)
				{
				case 0:
					neighX = floorTargetX; neighY = floorTargetY;
					neighCoefFrac = xFloorFrac * yFloorFrac;
					break;
				case 1:
					neighX = floorTargetX; neighY = roofTargetY;
					neighCoefFrac = xFloorFrac * yRoofFrac;
					break;
				case 2:
					neighX = roofTargetX;  neighY = floorTargetY;
					neighCoefFrac = xRoofFrac * yFloorFrac;
					break;
				case 3:
					neighX = roofTargetX;  neighY = roofTargetY;
					neighCoefFrac = xRoofFrac * yRoofFrac;
					break;

				default:
					REPORT_FAILURE("Unknown neigh type");
					break;
				}

				if(neighCoefFrac > FLT_EPSILON)
				{
					ENSURE(this->maskShape.InBounds(neighX, neighY));
					float neighCoef = neighCoefFrac * coef;

					uint targetAddr = this->maskShape.NodeIndex(neighX, neighY);
					
					InsertResult insertResult;
					if(flowType == 0)
					{
						VarCoef varCoefPair(targetAddr, neighCoef);
						insertResult = frameNeighbours[nodeAddr].insert(varCoefPair);
					}
					else
					{
						VarCoef varCoefPair(nodeAddr, neighCoef);
						insertResult = frameNeighbours[targetAddr].insert(varCoefPair);
					}

					if(insertResult.second == false)
					{
						insertResult.first->second += neighCoef;
					}
				}
			}
		}
	}
}
//*/

void VideoSolver::defineFrameEqs(int iChannel, int iFrame,
						 		 VarCoefList *prevFrameNeighbours,
								 VarCoefList *currFrameNeighbours,
								 VarCoefList *nextFrameNeighbours,						
								 SparseSolver &solver, const VideoSolverParams &params)
{
	ENSURE((iFrame >= 0)   && (iFrame < this->frameCount));
	ENSURE((iChannel >= 0) && (iChannel < this->imgShape.nBands));
	ENSURE(currFrameNeighbours != NULL);
	ENSURE((iFrame == 0) || (prevFrameNeighbours != NULL));
	ENSURE((iFrame == this->frameCount - 1) || (nextFrameNeighbours != NULL));

	//comute max neighbours and speed up by avoiding vectors

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	uint flowAddr  = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	for(int x = 0; x < this->maskShape.width;  x++, nodeAddr++, pixelAddr += this->imgShape.nBands, flowAddr += this->flowShape.nBands)
	{
		int varAddr = getNodeVar(iFrame, nodeAddr);
		ENSURE(varAddr >= 0);

		double rhs  = 0;
		double fxyz = this->currSolInitFrame[nodeAddr];			

		vector<int> eqVarAddr;
		vector<double> eqVarCoef;
		size_t eqSize = 0;
		eqSize += (prevFrameNeighbours != NULL) ? prevFrameNeighbours[nodeAddr].size() : 0;
		eqSize += (currFrameNeighbours != NULL) ? currFrameNeighbours[nodeAddr].size() : 0;
		eqSize += (nextFrameNeighbours != NULL) ? nextFrameNeighbours[nodeAddr].size() : 0;
		eqVarAddr.reserve(eqSize);
		eqVarCoef.reserve(eqSize);

		float varCoef        = 0;
		int currNodeVarIndex = -1;

		float currSpatialVal  = this->currSpatialFrame[pixelAddr + iChannel];
		float currTemporalVal = this->currTemporalFrame[pixelAddr + iChannel];
		for(int frameOffset = -1; frameOffset <= 1; frameOffset++)
		{				
			VarCoefList *frameNeighbours = NULL;
			float currChannelVal = 0;
			CFloatImage *neighbourImage = NULL;
			switch(frameOffset)
			{
			case -1:
				neighbourImage  = &this->prevTemporalFrame;
				frameNeighbours = prevFrameNeighbours;
				currChannelVal  = currTemporalVal;
				break;
			case 0:
				neighbourImage  = &this->currSpatialFrame;
				frameNeighbours = currFrameNeighbours;
				currChannelVal  = currSpatialVal;
				break;
			case 1:
				neighbourImage  = &this->nextTemporalFrame;
				frameNeighbours = nextFrameNeighbours;
				currChannelVal  = currTemporalVal;
				break;
			default:
				REPORT_FAILURE("Unknown offset");
				break;
			}

			int neighbourFrameIndex = iFrame + frameOffset;
			if ((neighbourFrameIndex >= this->frameCount) && (params.useEndConstraintFrame == false)) continue;
			if ((neighbourFrameIndex < 0) && (params.useStartConstraintFrame == false)) continue;

			VarCoefList &currNodeNeighbours = frameNeighbours[nodeAddr];
			for(VarCoefList::const_iterator currNodeNeighbour = currNodeNeighbours.begin();
				currNodeNeighbour != currNodeNeighbours.end();
				currNodeNeighbour++)
			{					
				uint neighbourNodeAddr  = currNodeNeighbour->first;
				uint neighbourPixelAddr = (neighbourNodeAddr * this->imgShape.nBands);
				float neighbourCoef     = currNodeNeighbour->second;

				if((neighbourFrameIndex == iFrame) && (neighbourNodeAddr == nodeAddr))
				{					
					eqVarAddr.push_back(varAddr);					
					eqVarCoef.push_back(0.0f);
					ENSURE(neighbourCoef == 0.0f);
					currNodeVarIndex = ((int)eqVarAddr.size()) - 1;
					continue;
				}

				varCoef += neighbourCoef;

				if(neighbourFrameIndex < 0)
				{
					ENSURE(params.useStartConstraintFrame == true);
					rhs -= (this->startConstraintFrameSol[neighbourNodeAddr] * neighbourCoef);
					rhs += (this->startConstraintFrame[neighbourPixelAddr + iChannel] - currChannelVal) * neighbourCoef;
					continue;
				}

				if(neighbourFrameIndex >= this->frameCount)
				{
					ENSURE(params.useEndConstraintFrame == true);
					rhs -= (this->endConstraintFrameSol[neighbourNodeAddr] * neighbourCoef);
					rhs += (this->endConstraintFrame[neighbourPixelAddr + iChannel] - currChannelVal) * neighbourCoef;
					continue;
				}

				int neighbourVarAddr = getNodeVar(neighbourFrameIndex, neighbourNodeAddr);
				ENSURE(neighbourVarAddr >= 0);
				if((this->SymmetricSolver == false) ||
				   (neighbourVarAddr >= varAddr))
				{
					eqVarAddr.push_back(neighbourVarAddr);
					eqVarCoef.push_back(neighbourCoef);
				}
				rhs += ((*neighbourImage)[neighbourPixelAddr + iChannel] - currChannelVal) * neighbourCoef;
			}
		}

		ENSURE(currNodeVarIndex != -1);
		ENSURE(varCoef > 0);
		eqVarCoef[currNodeVarIndex] = -(varCoef);			
		for(uint iEqVar = 0; iEqVar < eqVarAddr.size() - 1; iEqVar++)
		{
			ENSURE(eqVarAddr[iEqVar] < eqVarAddr[iEqVar + 1]);
		}
		ENSURE(eqVarAddr.size() == eqVarCoef.size());
		solver.SetRow(varAddr, (int) eqVarAddr.size(), fxyz, rhs, eqVarAddr, eqVarCoef);
	}
}

void VideoSolver::Solve(VideoSolverParams params)
{	
	initGlobals(params);

	VarCoefList *currFrameNeighbours = new VarCoefList[this->varsPerFrame];
	collectSpatialNeighbours(currFrameNeighbours, params);
	
	int numOfVars = getNumOfVars();	

	int startChannel = 0;
	int endChannel   = this->imgShape.nBands - 1;
	if(params.channel != -1)
	{
		ENSURE((params.channel >= 0) && (params.channel < this->imgShape.nBands));
		startChannel = endChannel = params.channel;
	}

	for(int iChannel = startChannel; iChannel <= endChannel; iChannel++)
	{
		Timer timer(true);
		printf("\nSolving Channel - %i\n", iChannel);

		loadConstraints(iChannel, params);
		SparseSolver solver(numOfVars, this->SymmetricSolver);

		printf("Defining equations..\n");        
		for(int iFrame = 0; iFrame < this->frameCount; iFrame++)
		{
			printf("."); fflush(stdout);
			loadCurrFrameContext(iFrame, iChannel, params);

			VarCoefList *prevFrameNeighbours = NULL;
			VarCoefList *nextFrameNeighbours = NULL;

			if((iFrame > 0) || (params.useStartConstraintFrame == true))
			{
				prevFrameNeighbours = new VarCoefList[this->varsPerFrame];
				ENSURE(prevFrameNeighbours != NULL);				
				//check
				//bool collectInNeighbours = (iFrame > 0) ? true : false;
				//collectTemporalNeighbours(prevFrameNeighbours, 
				//						  this->currToPrevFlow, true,
				//						  this->prevToCurrFlow, collectInNeighbours,
				//						  params);
				collectTemporalNeighbours(prevFrameNeighbours, 
										  this->currToPrevFlow, true,
										  this->prevToCurrFlow, false,
										  params);
			}
			if((iFrame < this->frameCount - 1) || (params.useEndConstraintFrame == true))
			{
				nextFrameNeighbours = new VarCoefList[this->varsPerFrame];
				ENSURE(nextFrameNeighbours != NULL);
				//check 
				//bool collectOutNeighbours = (iFrame < this->frameCount - 1) ? true : false;
				//collectTemporalNeighbours(nextFrameNeighbours, 
				//						  this->currToNextFlow, collectOutNeighbours,
				//						  this->nextToCurrFlow, true,
				//						  params);
				collectTemporalNeighbours(nextFrameNeighbours, 
										  this->currToNextFlow, false,
										  this->nextToCurrFlow, true,
										  params);
			}

			defineFrameEqs(iChannel, iFrame, 
						   prevFrameNeighbours, currFrameNeighbours, nextFrameNeighbours,
						   solver, params);

			delete[] prevFrameNeighbours;
			delete[] nextFrameNeighbours;		
			fflush(stdout);
		}		
		printf("\n");

		printf("Running laspack..\n"); fflush(stdout);
		solver.Solve(params.accuracy, params.maxIters);
		timer.DumpSeconds("Done");
		writeResults(iChannel, solver, params);
		fflush(stdout);
	}
	delete[] currFrameNeighbours;
}


void VideoSolver::writeResults(int channel, SparseSolver &solver, const VideoSolverParams &params)
{
	ENSURE((channel >= 0) && (channel < 3));

	double solOffset = 0;	
	if((params.useStartConstraintFrame == false) && (params.useEndConstraintFrame == false))
	{
		solOffset = (constrainedVoxelColor[channel] - solver.GetSolution(getNodeVar(0,0)));
	}	

	CFloatImage currFrame(this->maskShape);
	currFrame.ClearPixels();

	printf("Writing channel - %i..\n", channel);
	for(int iFrame = 0; iFrame < this->frameCount; iFrame++)
	{
		printf(".");
		string outName = getChannelResultName(iFrame, channel, params);

		uint nodeAddr = 0;
		for(int y = 0; y < this->maskShape.height; y++)
		for(int x = 0; x < this->maskShape.width;  x++, nodeAddr++)
		{
			int varAddr = getNodeVar(iFrame, nodeAddr);
			ENSURE(varAddr >= 0);
			currFrame[nodeAddr] = (float) (solver.GetSolution(varAddr) + solOffset);
		}

		ImageIO::WriteFile(currFrame, outName);
	}
	printf("\n  Done.\n");
}

void VideoSolver::combineResults(const VideoSolverParams &params)
{
	ENSURE(this->imgShape.nBands  == 3);

	CFloatImage currFrame(this->imgShape);
	printf("Combining results..\n");
	for(int iFrame = 0; iFrame < this->frameCount; iFrame++)
	{
		for(int iChannel = 0; iChannel < this->imgShape.nBands; iChannel++)
		{
			CFloatImage channelImg(this->maskShape);
			string channelImgFn = getChannelResultName(iFrame, iChannel, params);

			ImageIO::ReadFile(channelImg, channelImgFn);
			ENSURE(channelImg.Shape() == this->maskShape);

			ImageProcessing::CopyChannel(currFrame, channelImg, iChannel, 0);
		}

		string currFrameFN = getResultName(iFrame, params);
		ImageIO::WriteFile(currFrame, currFrameFN);

		printf(".");
		fflush(stdout);
	}
	printf("\n  Done.\n");
}

void VideoSolver::CombineResults(VideoSolverParams params)
{
	initGlobals(params);
	combineResults(params);
}
