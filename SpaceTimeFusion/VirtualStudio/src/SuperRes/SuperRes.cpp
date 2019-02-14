#include "SuperRes.h"
#include "ImageIO.h"
#include "Draw.h"
#include "Transform-3x3.h"
#include "Timer.h"

//check - turn ann to floating point
//check - dealloc search data
//check - use l1 dist

SuperRes::SuperRes(SuperResParams superResParams)
{
	this->params = superResParams;
	this->usePrevResult = false;

	CHECK_RANGE(this->params.patchOverlap, 0, this->params.patchSize - 1);
	ENSURE(this->params.largePatchSize >= this->params.patchSize);	
	ENSURE((this->params.largePatchSize - this->params.patchSize) % 2 == 0);	
}

void SuperRes::setupMatchKernel()
{
	ENSURE(this->imgShape.nBands != 0);
	CShape kernelShape(this->params.largePatchSize, 
					   this->params.largePatchSize, 
					   this->imgShape.nBands);
	this->matchKernel.ReAllocate(kernelShape);

	int largePatchHalfWidth = this->params.largePatchSize / 2;

	float denom = (largePatchHalfWidth * largePatchHalfWidth * 2) / log(0.25f);

	int nodeAddr = 0;
	for(float x = 0; x < this->params.largePatchSize; x++)
	for(float y = 0; y < this->params.largePatchSize; y++)
	{
		float diff;
		float dist = 0.0f;
		diff = abs(x - largePatchHalfWidth);
		dist += (diff * diff);
		diff = abs(y - largePatchHalfWidth);
		dist += (diff * diff);

		float weight = exp(dist / denom);

		for(int channel = 0; channel < this->imgShape.nBands; channel++, nodeAddr++)
		{
			//bug
			matchKernel[nodeAddr] = weight;
			//matchKernel[nodeAddr] = 1.0f;
		}
	}	
}

void SuperRes::ComputeFilter(CFloatImage targetImg)
{
	if(targetImg.Shape().nBands == 3)
	{
		targetImg = ColorHelper::RGBtoLuminance(targetImg);
	}

	this->targetImgs.push_back(targetImg);
	initGlobalVars();

	start();
	
	for(uint iFrame = 0; iFrame < this->targetImgs.size(); iFrame++)
	{
		generateResults(iFrame, true);
	}	

	freeGlobalVars();	
}

void SuperRes::UsePrevResult(CFloatImage &flowField, CFloatImage &warpResult)
{
	ENSURE(this->imgShape == warpResult.Shape());
	ENSURE(this->imgShape.SameIgnoringNBands(flowField.Shape()));
	ENSURE(flowField.Shape().nBands == 3);

	this->usePrevResult = true;
	this->flowField = flowField;
	this->warpResult = warpResult;
}


void SuperRes::SetTargetImg(CFloatImage targetImg)
{
	targetImg = ImageProcessing::BilateralFilter(targetImg, 3.0f, 0.1f, 3);	
	ImageIO::WriteFile(targetImg, "blur-" + this->params.outPrefix + ".tga");
	this->targetImgs.push_back(targetImg);

	initGlobalVars();
}

void SuperRes::ComputeSuperRes(CFloatImage targetImg)
{
	//bug
	//targetImg = createLowResImg(targetImg);
	targetImg = ImageProcessing::BilateralFilter(targetImg, 3.0f, 0.1f, 3);

	CFloatImage test1 = createHighFreqImg(targetImg);
	ImageProcessing::Operate(test1, test1, 0.5f, ImageProcessing::OP_ADD);
	ImageIO::WriteFile(test1, "1.tga");
	exit(0);
	
	ImageIO::WriteFile(targetImg, "blur-" + this->params.outPrefix + ".tga");

	this->targetImgs.push_back(targetImg);
	initGlobalVars();

	start();
	
	for(uint iFrame = 0; iFrame < this->targetImgs.size(); iFrame++)
	{
		generateResults(iFrame, false);
	}	

	freeGlobalVars();
}


void SuperRes::start()
{	
	Timer timer(true);

	printf("Generating labels...\n");
	for(uint iFrame = 0; iFrame < this->targetImgs.size(); iFrame++)
	{		
		generateLabels(iFrame);		
		printf("\n");
	}	
	timer.DumpSeconds("  Done.");

	printf("\nComputing datacosts...\n");
	for(uint iFrame = 0; iFrame < this->targetImgs.size(); iFrame++)
	{
		if(usePrevResult)
			augmentDataCosts(iFrame);

		//bug
		//computeDataCosts(iFrame, this->params.patchSize, false);
		//if(this->params.largePatchSize > this->params.patchSize)
		//{
		//	computeDataCosts(iFrame, this->params.largePatchSize, true, 0.5f);
		//}
		//normalizeDataCosts(iFrame);

		computeDataCosts(iFrame);
	}
	timer.DumpSeconds("\n  Done.");

	printf("\nComputing spatial smoothness data...\n");
	for(uint iFrame = 0; iFrame < this->targetImgs.size(); iFrame++)
	{
		computeSpatialSmoothnessData(iFrame);
	}
	timer.DumpSeconds("\n  Done.");

	printf("\nBP iterations...\n");
	for(int iter = 0; iter < this->params.bpIters; iter++)
	{
		//bug
		bpIter(0);
	}
	timer.DumpSeconds("\n  Done.");
}



void SuperRes::initGlobalVars()
{
	this->labels.resize(this->targetImgs.size());	
	this->datacosts.resize(this->targetImgs.size());
	this->spatialMsges.resize(this->targetImgs.size());
	this->prevSpatialMsges.resize(this->targetImgs.size());
	this->spatialMinDists.resize(this->targetImgs.size());
	this->spatialProbSum.resize(this->targetImgs.size());
	this->stdImgs.resize(this->targetImgs.size());

	ENSURE(this->targetImgs.size() > 0);
	this->imgShape = this->targetImgs[0].Shape();	

	this->labelShape.width  = 0;
	this->labelShape.height = 0;
	this->labelShape.nBands = 1;

	for(int y = 0; true; y += (this->params.patchSize - this->params.patchOverlap))
	{
		this->labelShape.height++;
		if(y >= this->imgShape.height)
			break;
	}

	for(int x = 0; true; x += (this->params.patchSize - this->params.patchOverlap))
	{
		this->labelShape.width++;
		if(x >= this->imgShape.width)
			break;
	}

	CShape stdShape = this->labelShape;
	stdShape.nBands = this->imgShape.nBands;


	for(uint iFrame = 0; iFrame < this->targetImgs.size(); iFrame++)
	{
		ENSURE(this->imgShape == this->targetImgs[iFrame].Shape());

		this->labels[iFrame].ReAllocate(this->labelShape);
		this->datacosts[iFrame].ReAllocate(this->labelShape);
		this->spatialMsges[iFrame].ReAllocate(this->labelShape);		
		this->prevSpatialMsges[iFrame].ReAllocate(this->labelShape);
		this->spatialMinDists[iFrame].ReAllocate(this->labelShape);
		this->spatialProbSum[iFrame].ReAllocate(this->labelShape);
		this->stdImgs[iFrame].ReAllocate(stdShape);
		this->spatialMinDists[iFrame].ClearPixels();
		this->spatialProbSum[iFrame].ClearPixels();
		this->stdImgs[iFrame].ClearPixels();

		uint labelAddr = 0;
		for(int y = 0; y < this->labelShape.height; y++)
		for(int x = 0; x < this->labelShape.width; x++, labelAddr++)
		{
			this->labels[iFrame][labelAddr]    = new ANNidx[this->params.maxLabels];
			this->datacosts[iFrame][labelAddr] = new ANNdist[this->params.maxLabels];

			this->spatialMsges[iFrame][labelAddr] = new float*[BP_Connectivity];
			this->prevSpatialMsges[iFrame][labelAddr] = new float*[BP_Connectivity];
			this->spatialMinDists[iFrame][labelAddr] = new float[BP_Connectivity];
			this->spatialProbSum[iFrame][labelAddr] = new float[BP_Connectivity];
			for(int iNeigh = 0; iNeigh < BP_Connectivity; iNeigh++)
			{
				int neighX = x + BP_DX[iNeigh];
				int neighY = y + BP_DY[iNeigh];
				if(this->labelShape.InBounds(neighX, neighY))
				{
					this->spatialMsges[iFrame][labelAddr][iNeigh] = new float[this->params.maxLabels];
					this->prevSpatialMsges[iFrame][labelAddr][iNeigh] = new float[this->params.maxLabels];
					float unitProb = 1.0f / this->params.maxLabels;
					for(int iElem = 0; iElem < this->params.maxLabels; iElem++)
					{
						this->prevSpatialMsges[iFrame][labelAddr][iNeigh][iElem] = unitProb;
						this->spatialMsges[iFrame][labelAddr][iNeigh][iElem] = unitProb;
					}
				}
				else
				{
					this->spatialMsges[iFrame][labelAddr][iNeigh] = NULL;
					this->prevSpatialMsges[iFrame][labelAddr][iNeigh] = NULL;
				}
			}
		}
	}	
}

void SuperRes::freeGlobalVars()
{
	for(uint iFrame = 0; iFrame < this->targetImgs.size(); iFrame++)
	{	
		uint labelAddr = 0;
		for(int y = 0; y < this->labelShape.height; y++)
		for(int x = 0; x < this->labelShape.width; x++, labelAddr++)
		{
			delete this->labels[iFrame][labelAddr];
			delete this->datacosts[iFrame][labelAddr];

			delete this->spatialMinDists[iFrame][labelAddr];
			delete this->spatialProbSum[iFrame][labelAddr];
			this->spatialMinDists[iFrame][labelAddr] = NULL;
			this->spatialProbSum[iFrame][labelAddr] = NULL;

			this->labels[iFrame][labelAddr]    = NULL;
			this->datacosts[iFrame][labelAddr] = NULL;

			for(int iNeigh = 0; iNeigh < BP_Connectivity; iNeigh++)
			{
				if(this->spatialMsges[iFrame][labelAddr][iNeigh] != NULL)
				{
					delete this->spatialMsges[iFrame][labelAddr][iNeigh];
					delete this->prevSpatialMsges[iFrame][labelAddr][iNeigh];
				}
			}

			delete this->spatialMsges[iFrame][labelAddr];
			delete this->prevSpatialMsges[iFrame][labelAddr];
			this->spatialMsges[iFrame][labelAddr] = NULL;
			this->prevSpatialMsges[iFrame][labelAddr] = NULL;
		}
	}

	annDeallocPts(this->dataPts);
	delete this->searchTree;
	//delete this->searchTree2;
}

void SuperRes::generateLabels(int iFrame)
{		
	int searchVecDim = this->params.largePatchSize * this->params.largePatchSize * this->imgShape.nBands;
	ANNpoint queryPt = annAllocPt(searchVecDim);

	CFloatImage &frameImg        = this->targetImgs[iFrame];
	CDlbPtrImage &frameDataCosts = this->datacosts[iFrame];
	CIntPtrImage &frameLabels    = this->labels[iFrame];
	CFloatImage frameImgHiFreq   = createHighFreqImg(frameImg); //bug - analogy

	int largePatchHalfWidth = this->params.largePatchSize / 2;


	int pixelShift = (this->params.patchSize - this->params.patchOverlap);
	uint nodeAddr = 0;
	for(int labelY = 0, y = 0; labelY < this->labelShape.height; labelY++, y += pixelShift)
	for(int labelX = 0, x = 0; labelX < this->labelShape.width;  labelX++, x += pixelShift, nodeAddr++)
	{
		CFloatImage currPatch = ImageProcessing::CropImageUsingReflection(frameImgHiFreq,
																		   x - largePatchHalfWidth,
																		   y - largePatchHalfWidth,
																		   this->params.largePatchSize, this->params.largePatchSize);
		

		for(int channel = 0; channel < imgShape.nBands; channel++)
		{
			if(this->params.normalizeMeanStdDev == false)
			{
				//float channelMean = ImageProcessing::GetMean(currPatch, channel);
				//channelMean = abs(channelMean) + SuperRes_MinMean;
				//ImageProcessing::Operate(currPatch, currPatch, channelMean, ImageProcessing::OP_DIV, channel);

				float channelMean, channelStdDev;
				ImageProcessing::GetMeanAndDev(currPatch, channel, channelMean, channelStdDev);
				ImageProcessing::Operate(currPatch, currPatch, channelStdDev + SuperRes_MatchEpsilon, ImageProcessing::OP_DIV, channel);
				this->stdImgs[iFrame].Pixel(labelX, labelY, channel) = channelStdDev;
			}
			else
			{
				ImageProcessing::RemapMeanAndDev(currPatch, channel, 0.0f, 1.0f);
			}
		}

		createLowResLargePatch(currPatch, queryPt);		
		//for(int iElem = 0; iElem < searchVecDim; iElem++)
		//{
		//	queryPt[iElem] = (ANNcoord) (this->matchKernel[iElem] * currPatch[iElem]);
		//}

		if(usePrevResult == false)
		{
			this->searchTree->annkSearch(queryPt,
										 this->params.maxLabels,
										 frameLabels[nodeAddr],
										 frameDataCosts[nodeAddr],
										 this->params.annErrorBound);

			//this->searchTree->annkSearch(queryPt,
			//							 this->params.maxLabels / 2,
			//							 frameLabels[nodeAddr],
			//							 frameDataCosts[nodeAddr],
			//							 this->params.annErrorBound);


			//this->searchTree2->annkSearch(queryPt,
			//				 			  this->params.maxLabels / 2,
			//							  frameLabels[nodeAddr] + (this->params.maxLabels / 2),
			//							  frameDataCosts[nodeAddr] + (this->params.maxLabels / 2),
			//							  this->params.annErrorBound);

		}
		else
		{
			ENSURE(this->params.maxLabels % 2 == 0);

			this->searchTree->annkSearch(queryPt,
										 this->params.maxLabels / 2,
										 frameLabels[nodeAddr],
										 frameDataCosts[nodeAddr],
										 this->params.annErrorBound);

			CFloatImage flowPatch = ImageProcessing::CropImageUsingReflection(this->warpResult,
																			   x - largePatchHalfWidth,
																			   y - largePatchHalfWidth,
																			   this->params.largePatchSize, this->params.largePatchSize);
			
			createLowResLargePatch(flowPatch, queryPt);	
			//for(int iElem = 0; iElem < searchVecDim; iElem++)
			//{
			//	queryPt[iElem] = (ANNcoord) (this->matchKernel[iElem] * flowPatch[iElem]);
			//}

			this->searchTree->annkSearch(queryPt,
										 this->params.maxLabels / 2,
										 frameLabels[nodeAddr] + (this->params.maxLabels / 2),
										 frameDataCosts[nodeAddr] + (this->params.maxLabels / 2),
										 this->params.annErrorBound);
		}

		if(x == 0) printf(".");
	}	

	annDeallocPt(queryPt);
}

void SuperRes::augmentDataCosts(int iFrame)
{
	CDlbPtrImage &frameDataCost = this->datacosts[iFrame];
	CIntPtrImage &frameLabels = this->labels[iFrame];

	int patchNodeCount = this->params.largePatchSize * this->params.largePatchSize;
	CFloatImage distImg(this->params.largePatchSize, this->params.largePatchSize, this->imgShape.nBands);

	int largePatchHW = this->params.largePatchSize / 2;

	int pixelShift = (this->params.patchSize - this->params.patchOverlap);
	uint nodeAddr = 0;
	for(int labelY = 0, y = 0; labelY < this->labelShape.height; labelY++, y += pixelShift)
	for(int labelX = 0, x = 0; labelX < this->labelShape.width;  labelX++, x += pixelShift, nodeAddr++)
	{
		CFloatImage warpPatch = ImageProcessing::CropImageUsingReflection(this->warpResult,
																		   x - largePatchHW, y - largePatchHW,
																		   this->params.largePatchSize, this->params.largePatchSize);

		CFloatImage flowPatch = ImageProcessing::CropImageUsingReflection(this->flowField,
																		   x - largePatchHW, y - largePatchHW,
																		   this->params.largePatchSize, this->params.largePatchSize);
		
		double minDist = FLT_MAX;
		int minDistIndex = -1;
		for(int iLabel = 0; iLabel < this->params.maxLabels; iLabel++)
		{
			int currLabel = frameLabels[nodeAddr][iLabel];
			CFloatImage labelPatch = getHiResLargePatch(currLabel);

			ImageProcessing::Operate(distImg, warpPatch, labelPatch, ImageProcessing::OP_SUB);
			ImageProcessing::Operate(distImg, distImg, distImg, ImageProcessing::OP_MUL);

			float dist = 0.0f;
			for(int iNode = 0; iNode < patchNodeCount; iNode++)
			for(int channel = 0; channel < this->imgShape.nBands; channel++)
			{
				//bug
				dist += distImg[iNode * this->imgShape.nBands + channel] *
					    flowPatch[iNode * 3 + 2];			

				//dist += distImg[iNode * this->imgShape.nBands + channel];
			}
			
			dist = sqrt(dist);

			frameDataCost[nodeAddr][iLabel] = (0.1 * frameDataCost[nodeAddr][iLabel]) + 
				                               (0.9 * dist);

			if(frameDataCost[nodeAddr][iLabel] < minDist)
			{
				minDist = frameDataCost[nodeAddr][iLabel];
				minDistIndex = iLabel;
			}
		}

		ENSURE(minDistIndex != -1);

		double tempDist = frameDataCost[nodeAddr][minDistIndex];
		frameDataCost[nodeAddr][minDistIndex] = frameDataCost[nodeAddr][0];
		frameDataCost[nodeAddr][0] = tempDist;

		for(int iLabel = 1; iLabel < this->params.maxLabels; iLabel++)
		{
			ENSURE(frameDataCost[nodeAddr][iLabel] >= frameDataCost[nodeAddr][0]);
		}
	}
}

void SuperRes::normalizeDataCosts(int iFrame)
{
	CDlbPtrImage &frameDataCosts = this->datacosts[iFrame];

	uint nodeAddr = 0;
	for(int labelY = 0; labelY < this->labelShape.height; labelY++)
	for(int labelX = 0; labelX < this->labelShape.width;  labelX++, nodeAddr++)
	{
		double probSum = FLT_EPSILON;
		for(int iLabel = 0; iLabel < this->params.maxLabels; iLabel++)
		{
			probSum +=  frameDataCosts[nodeAddr][iLabel];
		}

		for(int iLabel = 0; iLabel < this->params.maxLabels; iLabel++)
		{
			frameDataCosts[nodeAddr][iLabel] /= probSum;
		}
	}
}


void SuperRes::computeDataCosts(int iFrame, int patchSize, bool blendNewDataCost, float newDatacostW)
{
	CHECK_RANGE(newDatacostW, 0.0f, 1.0f);

	float noiseStd = this->params.dataCostNoiseStdDev;
	noiseStd *= this->params.largePatchSize / (float) patchSize;
	float dataCostDenom = -2.0f * noiseStd * noiseStd; 

	CDlbPtrImage &frameDataCosts = this->datacosts[iFrame];
	CIntPtrImage &frameLabels    = this->labels[iFrame];

	float *nodeDists = new float[this->params.maxLabels];

	CFloatImage &frameImg        = this->targetImgs[iFrame];
	CFloatImage frameImgHiFreq   = createHighFreqImg(frameImg); //bug - analogy

	int patchHalfWidth = patchSize / 2;

	int pixelShift = (this->params.patchSize - this->params.patchOverlap);
	uint nodeAddr = 0;
	for(int labelY = 0, y = 0; labelY < this->labelShape.height; labelY++, y += pixelShift)
	for(int labelX = 0, x = 0; labelX < this->labelShape.width;  labelX++, x += pixelShift, nodeAddr++)
	{
		CFloatImage matchPatch = ImageProcessing::CropImageUsingReflection(frameImgHiFreq,
																		   x - patchHalfWidth, y - patchHalfWidth,
																		   patchSize, patchSize);			
		for(int channel = 0; channel < imgShape.nBands; channel++)
		{
			if(this->params.normalizeMeanStdDev == false)
			{
				//float channelMean = ImageProcessing::GetMean(currPatch, channel);
				//channelMean = abs(channelMean) + SuperRes_MinMean;
				//ImageProcessing::Operate(currPatch, currPatch, channelMean, ImageProcessing::OP_DIV, channel);

				float channelMean, channelStdDev;
				ImageProcessing::GetMeanAndDev(matchPatch, channel, channelMean, channelStdDev);
				ImageProcessing::Operate(matchPatch, matchPatch, channelStdDev + SuperRes_MatchEpsilon, ImageProcessing::OP_DIV, channel);
			}
			else
			{
				ImageProcessing::RemapMeanAndDev(matchPatch, channel, 0.0f, 1.0f);
			}
		}

		float minDist = FLT_MAX;		
		for(int iLabel = 0; iLabel < this->params.maxLabels; iLabel++)
		{
			float dist = getPatchDist(matchPatch, frameLabels[nodeAddr][iLabel], patchSize);
			nodeDists[iLabel] = dist;
			SET_IF_LT(minDist, dist);
		}
		ENSURE(minDist != FLT_MAX);

		for(int iLabel = 0; iLabel < this->params.maxLabels; iLabel++)
		{
			float nodeDist = nodeDists[iLabel];
			float normNodeDist = nodeDist / minDist;
			float probSpaceDist = 1.0f - (1.0f / normNodeDist);
			CHECK_RANGE(probSpaceDist, 0.0f, 1.0f);

			float nodeDatacost = exp(probSpaceDist * probSpaceDist / dataCostDenom);

			if(blendNewDataCost == false)
				frameDataCosts[nodeAddr][iLabel] = nodeDatacost;
			else
			{
			  double newDataCost = pow(frameDataCosts[nodeAddr][iLabel] * nodeDatacost, 0.5);
			  frameDataCosts[nodeAddr][iLabel] =  (newDataCost * newDatacostW) +
												   frameDataCosts[nodeAddr][iLabel] * (1.0f - newDatacostW);
			}
		}		
	}

	delete nodeDists;
}

float SuperRes::getPatchDist(CFloatImage &matchPatch, int patchID, int patchSize)
{
	//int patchHW = patchSize / 2;

	//CFloatImage matchPatch = ImageProcessing::CropImageUsingReflection(matchImg,
	//																  x - patchHW, y - patchHW,
	//																  patchSize, patchSize);
	//	
	//for(int channel = 0; channel < imgShape.nBands; channel++)
	//{
	//	if(this->params.normalizeMeanStdDev == false)
	//	{
	//		//float channelMean = ImageProcessing::GetMean(currPatch, channel);
	//		//channelMean = abs(channelMean) + SuperRes_MinMean;
	//		//ImageProcessing::Operate(currPatch, currPatch, channelMean, ImageProcessing::OP_DIV, channel);

	//		float channelMean, channelStdDev;
	//		ImageProcessing::GetMeanAndDev(matchPatch, channel, channelMean, channelStdDev);
	//		ImageProcessing::Operate(matchPatch, matchPatch, channelStdDev + SuperRes_MatchEpsilon, ImageProcessing::OP_DIV, channel);
	//	}
	//	else
	//	{
	//		ImageProcessing::RemapMeanAndDev(matchPatch, channel, 0.0f, 1.0f);
	//	}
	//}

	CFloatImage trainPatch = getLowResPatch(patchID, patchSize);

	CFloatImage distImg(patchSize, patchSize, this->imgShape.nBands);
	ImageProcessing::Operate(distImg, trainPatch, matchPatch, ImageProcessing::OP_SUB);
	ImageProcessing::Operate(distImg, distImg, distImg, ImageProcessing::OP_MUL);
	
	float dist = 0.0f;

	int nodeCount = patchSize * patchSize * this->imgShape.nBands;
	for(int iNode = 0; iNode < nodeCount; iNode++)
	{
		dist += this->matchKernel[iNode] * distImg[iNode];
	}

	dist = sqrt(dist);

	return dist + SuperRes_MinDataCostDist;
}

///*
void SuperRes::computeDataCosts(int iFrame)
{
	float dataCostDenom = -2.0f * this->params.dataCostNoiseStdDev * this->params.dataCostNoiseStdDev; 

	CDlbPtrImage &frameDataCosts = this->datacosts[iFrame];

	uint nodeAddr = 0;
	for(int y = 0; y < this->labelShape.height; y++)
	for(int x = 0; x < this->labelShape.width; x++, nodeAddr++)
	{
		float probSum = 0.0f;

		float minDist = (float) sqrt(frameDataCosts[nodeAddr][0]) + SuperRes_MinDataCostDist;
		for(int iLabel = 0; iLabel < this->params.maxLabels; iLabel++)
		{
			float nodeDist = (float) sqrt(frameDataCosts[nodeAddr][iLabel]) + SuperRes_MinDataCostDist;
			float normNodeDist = nodeDist / minDist;
			float probSpaceDist = 1.0f - (1.0f / normNodeDist);
			if((probSpaceDist < 0.0f) ||
				(probSpaceDist > 1.0f))

			{
				PRINT_SCI(probSpaceDist);
				PRINT_SCI(nodeDist);
				PRINT_SCI(minDist);				
			}
			CHECK_RANGE(probSpaceDist, 0.0f, 1.0f);

			frameDataCosts[nodeAddr][iLabel] = exp(probSpaceDist * probSpaceDist / dataCostDenom);

			probSum += (float) frameDataCosts[nodeAddr][iLabel];
		}

		for(int iLabel = 0; iLabel < this->params.maxLabels; iLabel++)
		{
			frameDataCosts[nodeAddr][iLabel] /= probSum;
		}
	}
}
//*/

void SuperRes::computeSpatialSmoothnessData(int iFrame)
{
	CIntPtrImage &frameLabels = this->labels[iFrame];
	CFltPtrImage &frameMinDists = this->spatialMinDists[iFrame];
	CFltPtrImage &frameProbSums = this->spatialProbSum[iFrame];

	uint nodeAddr = 0;
	for(int y = 0; y < this->labelShape.height; y++)
	for(int x = 0; x < this->labelShape.width; x++, nodeAddr++)
	for(int iNeigh = 0; iNeigh < BP_Connectivity; iNeigh++)
	{
		frameMinDists[nodeAddr][iNeigh] = FLT_MAX;
		frameProbSums[nodeAddr][iNeigh] = 0.0f;

	}

	nodeAddr = 0;
	for(int y = 0; y < this->labelShape.height; y++)
	for(int x = 0; x < this->labelShape.width; x++, nodeAddr++)
	{
		int *nodeLabels = frameLabels[nodeAddr];
		for(int iNeigh = 0; iNeigh < BP_Connectivity; iNeigh++)
		{
			int neighX = x + BP_DX[iNeigh];
			int neighY = y + BP_DY[iNeigh];
			if(this->labelShape.InBounds(neighX, neighY) == true)
			{
				uint neighNodeAddr = this->labelShape.NodeIndex(neighX, neighY);
				if(neighNodeAddr < nodeAddr) continue;

				int neighComplementIndex = BP_NEIGH_COMPLEMENT[iNeigh];

				int *neighNodeLabels = frameLabels[neighNodeAddr];

				for(int jElem = 0; jElem < this->params.maxLabels; jElem++)
				{
					int nodeLabel = nodeLabels[jElem];					
					for(int iElem = 0; iElem < this->params.maxLabels; iElem++)
					{
						int neighNodeLabel = neighNodeLabels[iElem];
						float currDist = computeSpatialSmoothnessDist(nodeLabel, neighNodeLabel, (BoundaryType) iNeigh, nodeAddr, neighNodeAddr, iFrame);

						if(frameMinDists[nodeAddr][iNeigh] > currDist)
						{
							frameMinDists[nodeAddr][iNeigh] = currDist;
							frameMinDists[neighNodeAddr][neighComplementIndex] = currDist;
						}						
					}
				}
			}
		}
	}

	float spatialProbDenom = -2.0f * this->params.spatialCostNoiseStdDev * this->params.spatialCostNoiseStdDev;

	nodeAddr = 0;
	for(int y = 0; y < this->labelShape.height; y++)
	for(int x = 0; x < this->labelShape.width; x++, nodeAddr++)
	{
		int *nodeLabels = frameLabels[nodeAddr];
		for(int iNeigh = 0; iNeigh < BP_Connectivity; iNeigh++)
		{
			int neighX = x + BP_DX[iNeigh];
			int neighY = y + BP_DY[iNeigh];
			if(this->labelShape.InBounds(neighX, neighY) == true)
			{
				uint neighNodeAddr = this->labelShape.NodeIndex(neighX, neighY);
				if(neighNodeAddr < nodeAddr) continue;

				int neighComplementIndex = BP_NEIGH_COMPLEMENT[iNeigh];

				int *neighNodeLabels = frameLabels[neighNodeAddr];

				float minDist = frameMinDists[nodeAddr][iNeigh];

				for(int jElem = 0; jElem < this->params.maxLabels; jElem++)
				{
					int nodeLabel = nodeLabels[jElem];					
					for(int iElem = 0; iElem < this->params.maxLabels; iElem++)
					{
						int neighNodeLabel = neighNodeLabels[iElem];
						float currDist = computeSpatialSmoothnessDist(nodeLabel, neighNodeLabel, (BoundaryType) iNeigh, nodeAddr, neighNodeAddr, iFrame);
						ENSURE(currDist >= minDist);

						float probDist = 1.0f - (minDist / currDist);
						CHECK_RANGE(probDist, 0.0f, 1.0f);
						float prob = exp(probDist * probDist / spatialProbDenom);						
						frameProbSums[nodeAddr][iNeigh] += prob;
						frameProbSums[neighNodeAddr][neighComplementIndex] += prob;
					}
				}
			}
		}
	}


	/*
	nodeAddr = 0;
	for(int y = 0; y < this->labelShape.height; y++)
	for(int x = 0; x < this->labelShape.width; x++, nodeAddr++)
	{
		int *nodeLabels = frameLabels[nodeAddr];
		for(int iNeigh = 0; iNeigh < BP_Connectivity; iNeigh++)
		{
			int neighX = x + BP_DX[iNeigh];
			int neighY = y + BP_DY[iNeigh];
			if(this->labelShape.InBounds(neighX, neighY) == true)
			{
				uint neighNodeAddr = this->labelShape.NodeIndex(neighX, neighY);

				int neighComplementIndex = BP_NEIGH_COMPLEMENT[iNeigh];

				int *neighNodeLabels = frameLabels[neighNodeAddr];

				float minDist = frameMinDists[nodeAddr][iNeigh];

				float probSum = 0.0f;
				float probNorm = frameProbSums[nodeAddr][iNeigh];
				for(int jElem = 0; jElem < this->params.maxLabels; jElem++)
				{
					int nodeLabel = nodeLabels[jElem];					
					for(int iElem = 0; iElem < this->params.maxLabels; iElem++)
					{
						int neighNodeLabel = neighNodeLabels[iElem];
						float currDist = computeSpatialSmoothnessDist(nodeLabel, neighNodeLabel, (BoundaryType) iNeigh);
						
						float probDist = 1.0f - (minDist / currDist);
						float prob = exp(probDist * probDist / spatialProbDenom) / probNorm;
						probSum += prob;
					}
				}

				ENSURE(abs(probSum - 1.0f) < 0.001f);
			}
		}
	}
	*/
}

float SuperRes::computeSpatialSmoothnessDist(int patchID1, int patchID2, BoundaryType boundaryType, uint patchTargetAddr1, uint patchTargetAddr2, int iFrame)
{
	ENSURE(this->params.patchOverlap == 1);

	CFloatImage patch1 = getHiResPatch(patchID1);
	CFloatImage patch2 = getHiResPatch(patchID2);

	for(int channel = 0; channel < this->imgShape.nBands; channel++)
	{
		ImageProcessing::Operate(patch1, patch1, 
								 this->stdImgs[iFrame][patchTargetAddr1 * this->imgShape.nBands + channel] + SuperRes_RenderEpsilon,
								 ImageProcessing::OP_MUL, channel);

		ImageProcessing::Operate(patch2, patch2, 
								 this->stdImgs[iFrame][patchTargetAddr2 * this->imgShape.nBands + channel] + SuperRes_RenderEpsilon,
								 ImageProcessing::OP_MUL, channel);
	}

	int patch1_ImgI, patch1_x, patch1_y;
	int patch2_ImgI, patch2_x, patch2_y;
	getPatchLoc(patchID1, patch1_ImgI, patch1_x, patch1_y);
	getPatchLoc(patchID2, patch2_ImgI, patch2_x, patch2_y);
	int expectedX = patch1_x, expectedY = patch1_y;

	uint pixelAddrIncr;
	uint pixelAddrPatch1, pixelAddrPatch2;

	switch(boundaryType)
	{
	case BT_LEFT:
		pixelAddrPatch1 = 0;
		pixelAddrPatch2 = patch2.PixelIndex(this->params.patchSize - 1, 0, 0);
		pixelAddrIncr = this->params.patchSize * this->imgShape.nBands;

		expectedX -= (this->params.patchSize - this->params.patchOverlap);
		break;

	case BT_RIGHT:
		pixelAddrPatch1 = patch1.PixelIndex(this->params.patchSize - 1, 0, 0);
		pixelAddrPatch2 = 0;
		pixelAddrIncr = this->params.patchSize * this->imgShape.nBands;
	
		expectedX += (this->params.patchSize - this->params.patchOverlap);
		break;		

	case BT_TOP:
		pixelAddrPatch1 = 0;
		pixelAddrPatch2 = patch2.PixelIndex(0, this->params.patchSize - 1, 0);
		pixelAddrIncr = this->imgShape.nBands;		

		expectedY += (this->params.patchSize - this->params.patchOverlap);
		break;

	case BT_BOTTOM:
		pixelAddrPatch1 = patch1.PixelIndex(0, this->params.patchSize - 1, 0);
		pixelAddrPatch2 = 0;
		pixelAddrIncr = this->imgShape.nBands;		

		expectedY -= (this->params.patchSize - this->params.patchOverlap);
		break;

	default:
		REPORT_FAILURE("Unknown boundary type");
		break;
	}

	float dist = 0.0f;
	for(int iPixel = 0; iPixel < this->params.patchSize; iPixel++)
	{
		for(int channel = 0; channel < this->imgShape.nBands; channel++)
		{
			float diff = ANN_DIFF(patch1[pixelAddrPatch1 + channel],
								  patch2[pixelAddrPatch2 + channel]);
			dist += ANN_POW(diff);			
		}
		pixelAddrPatch1 += pixelAddrIncr;
		pixelAddrPatch2 += pixelAddrIncr;
	}
	
	dist = ANN_ROOT(dist) + SuperRes_MinSpatialSmoothnessCostDist;

	float sDist;
	if(patch1_ImgI == patch2_ImgI)
	{
		sDist = sqrt((((float)expectedX - patch2_x) * (expectedX - patch2_x)) +
		               ((expectedY - patch2_y) * (expectedY - patch2_y)));
		sDist = min(sDist, this->params.patchSize * 2.0f);
	}
	else
	{
		sDist = this->params.patchSize * 2.0f;
	}	

	dist += sDist;

	return dist;
}


void SuperRes::generateResults(int iFrame, bool standAloneResult)
{
	CFloatImage &frameImg      = this->targetImgs[iFrame];
	CFloatImage frameStatImg;	
	CIntPtrImage &frameLabels  = this->labels[iFrame];

	CFloatImage uvImg(this->targetImgs[0].Shape());
	uvImg.ClearPixels();

	CFloatImage freqResult(this->imgShape);
	CFloatImage unNormFreqResult(this->imgShape);
	unNormFreqResult.ClearPixels();

	//check - use this code in generateLabels()
	if(standAloneResult == false)
		frameStatImg = createHighFreqImg(frameImg);
	else
		frameStatImg = frameImg;

	CFloatImage resultImg(this->imgShape);
	CFloatImage resultImg_notSmooth(this->imgShape);
	CFloatImage resultImgUnNorm(this->imgShape);
	resultImgUnNorm.ClearPixels();
	resultImg.ClearPixels();
	resultImg_notSmooth.ClearPixels();

	CFloatImage stdImg(this->imgShape);
	stdImg.ClearPixels();

	int patchHalfWidth = this->params.patchSize / 2;
	int largePatchHalfWidth = this->params.largePatchSize / 2;

	float *meanIncr   = new float[this->imgShape.nBands];
	float *stdDevIncr = new float[this->imgShape.nBands];

	int pixelShift = (this->params.patchSize - this->params.patchOverlap);
	uint nodeAddr = 0;

	for(int labelY = 0, y = 0; labelY < this->labelShape.height; labelY++, y += pixelShift)
	for(int labelX = 0, x = 0; labelX < this->labelShape.width;  labelX++, x += pixelShift, nodeAddr++)
	{
		//bug
		//CFloatImage currPatch = ImageProcessing::CropImageUsingReflection(frameStatImg,
		//																   x - patchHalfWidth,
		//																   y - patchHalfWidth,
		//																   this->params.patchSize, this->params.patchSize);

		//CFloatImage currPatch = ImageProcessing::CropImageUsingReflection(frameStatImg,
		//																   x - largePatchHalfWidth,
		//																   y - largePatchHalfWidth,
		//																   this->params.patchSize, this->params.patchSize);


		int currLabel = getLabel(iFrame, nodeAddr);
		int currLabel_notSmooth = frameLabels[nodeAddr][0];

		CFloatImage hiResPatch = getHiResPatch(currLabel);
		CFloatImage hiResPatch_notSmooth = getHiResPatch(currLabel_notSmooth);
		getPatchStatsIncr(currLabel, meanIncr, stdDevIncr);

		int patchI, patchU, patchV;
		getPatchLoc(currLabel, patchU, patchV, patchI);

		CFloatImage unNormHiResPatch = ImageProcessing::CropImageUsingReflection(this->aPrimeImgs[patchI],
																			     patchU - patchHalfWidth, patchV - patchHalfWidth, 
																				 this->params.patchSize, this->params.patchSize);

		uint patchPixelAddr = 0;		
		for(int yy = y - patchHalfWidth; yy <= y + patchHalfWidth; yy++)
		for(int xx = x - patchHalfWidth; xx <= x + patchHalfWidth; xx++)
		{
			if(this->imgShape.InBounds(xx, yy) == true)
			{
				for(int channel = 0; channel < imgShape.nBands; channel++, patchPixelAddr++)
				{
					freqResult.Pixel(xx, yy, channel) = hiResPatch[patchPixelAddr];
					unNormFreqResult.Pixel(xx, yy, channel) = unNormHiResPatch[patchPixelAddr];					
				}
				uvImg.Pixel(xx, yy, 0) = (float) patchU;
				uvImg.Pixel(xx, yy, 1) = (float) patchV;
			}
		}

		float channelStdDevs[3];
		for(int i = 0; i <= 1; i++)
		{
			CFloatImage matchingPatch;
			if(i == 0)
				matchingPatch = hiResPatch;
			else
				matchingPatch = hiResPatch_notSmooth;

			for(int channel = 0; channel < this->imgShape.nBands; channel++)
			{
				if(this->params.normalizeMeanStdDev == false)
				{
					//float channelMean = ImageProcessing::GetMean(currPatch, channel);
					////bug
					////channelMean = abs(channelMean) + SuperRes_MinMean;
					//channelMean = abs(channelMean);
					//ImageProcessing::Operate(matchingPatch, matchingPatch, channelMean, ImageProcessing::OP_MUL, channel);

					//float channelMean, channelStdDev;
					//ImageProcessing::GetMeanAndDev(currPatch, channel, channelMean, channelStdDev);
					//channelStdDev += SuperRes_RenderEpsilon;
					//channelStdDevs[channel] = channelStdDev;
					//ImageProcessing::Operate(matchingPatch, matchingPatch, channelStdDev, ImageProcessing::OP_MUL, channel);


					ImageProcessing::Operate(matchingPatch, matchingPatch, 
									this->stdImgs[iFrame].Pixel(labelX, labelY, channel) + SuperRes_RenderEpsilon, 
									ImageProcessing::OP_MUL, channel);
				}
				else
				{
					REPORT_FAILURE("Not implemented");
					//float channelMean, channelStdDev;
					//ImageProcessing::GetMeanAndDev(currPatch, channel, channelMean, channelStdDev);
					//channelStdDev += FLT_EPSILON;

					////channelMean += meanIncr[channel];
					////channelStdDev += stdDevIncr[channel];

					//ImageProcessing::RemapMeanAndDev(matchingPatch, channel, channelMean, channelStdDev);
				}
			}
		}

		patchPixelAddr = 0;		
		for(int yy = y - patchHalfWidth; yy <= y + patchHalfWidth; yy++)
		for(int xx = x - patchHalfWidth; xx <= x + patchHalfWidth; xx++)
		{
			if(this->imgShape.InBounds(xx, yy) == true)
			{
				for(int channel = 0; channel < imgShape.nBands; channel++, patchPixelAddr++)
				{
					resultImg.Pixel(xx, yy, channel) = hiResPatch[patchPixelAddr];
					resultImg_notSmooth.Pixel(xx, yy, channel) = hiResPatch_notSmooth[patchPixelAddr];
					stdImg.Pixel(xx, yy, channel) = channelStdDevs[channel];
				}
			}
		}
	}

	CFloatImage freqAdded = resultImg.Clone();
	ImageProcessing::Operate(freqAdded, freqAdded, 0.5f, ImageProcessing::OP_ADD);
	ImageIO::WriteFile(freqAdded, "freq-" + this->params.outPrefix + ".tga");

	if(standAloneResult == false)
	{
		ImageProcessing::Operate(resultImg, resultImg, frameImg, ImageProcessing::OP_ADD);
		ImageProcessing::Operate(resultImg_notSmooth, resultImg_notSmooth, frameImg, ImageProcessing::OP_ADD);
		ImageProcessing::Operate(resultImgUnNorm, unNormFreqResult, frameImg, ImageProcessing::OP_ADD);
		ImageIO::WriteFile(resultImgUnNorm, "result-unnorm-" + this->params.outPrefix + ".tga");

	}

	delete meanIncr;
	delete stdDevIncr;

	//char fileName[1024];
	//sprintf(fileName, "-result-%04i.tga", iFrame);
	ImageIO::WriteFile(resultImg, "result-" + this->params.outPrefix + ".tga");
	//sprintf(fileName, "-result_NS-%04i.tga", iFrame);
	ImageIO::WriteFile(resultImg_notSmooth, "result_NS-" + this->params.outPrefix + ".tga");

	ImageIO::WriteRaw(freqResult, this->params.outPrefix + ".raw");

	ImageProcessing::Operate(uvImg, uvImg, 255.0f, ImageProcessing::OP_DIV);
	ImageIO::WriteFile(uvImg, "uvImg-" + this->params.outPrefix + ".tga");

	ImageIO::WriteFile(unNormFreqResult, "unnorm-freq-" + this->params.outPrefix + ".tga");

	ImageProcessing::Operate(stdImg, stdImg, 5.0f, ImageProcessing::OP_MUL);
	ImageIO::WriteFile(stdImg, "stdImg-" + this->params.outPrefix + ".tga");
}


//todo - speed up
CFloatImage SuperRes::getHiResLargePatch(int patchID)
{
	CShape patchShape(this->params.largePatchSize, this->params.largePatchSize, this->imgShape.nBands);
	CFloatImage patch(patchShape);

	int patchVecDims = this->params.largePatchSize * this->params.largePatchSize * this->imgShape.nBands;

	for(int iElem = 0; iElem < patchVecDims; iElem++)
	{
		patch[iElem] = (float) this->dataPts[patchID][iElem + patchVecDims];
	}

	return patch;
}


void SuperRes::createLowResLargePatch(CFloatImage &patch, ANNpoint annPoint)
{
	int xyOffset = (this->params.largePatchSize - this->params.patchSize) / 2;

	int iElemLarge = this->params.patchSize * this->params.patchSize * this->imgShape.nBands;
	int iElemSmall = 0;
	uint pixelAddr = 0;
	for(int y = 0; y <  this->params.largePatchSize; y++)
	for(int x = 0; x <  this->params.largePatchSize; x++)
	{
		if((y < xyOffset) ||
		   (y >= xyOffset + this->params.patchSize) ||
		   (x < xyOffset) ||		   
		   (x >= xyOffset + this->params.patchSize))
		{
			for(int channel = 0; channel <  this->imgShape.nBands; channel++, iElemLarge++, pixelAddr++)
			{
				annPoint[iElemLarge] = (ANNcoord) patch[pixelAddr] * this->matchKernel[pixelAddr];
			}
		}
		else
		{
			for(int channel = 0; channel <  this->imgShape.nBands; channel++, iElemSmall++, pixelAddr++)
			{
				annPoint[iElemSmall] = (ANNcoord) patch[pixelAddr] * this->matchKernel[pixelAddr];
			}
		}
	}
}

///*
CFloatImage SuperRes::getLowResLargePatch(int patchID)
{
	CShape patchShape(this->params.largePatchSize, this->params.largePatchSize, this->imgShape.nBands);
	CFloatImage patch(patchShape);

	int xyOffset = (this->params.largePatchSize - this->params.patchSize) / 2;

	int iElemLarge = this->params.patchSize * this->params.patchSize * this->imgShape.nBands;
	int iElemSmall = 0;
	uint pixelAddr = 0;
	for(int y = 0; y <  this->params.largePatchSize; y++)
	for(int x = 0; x <  this->params.largePatchSize; x++)
	{
		if((y < xyOffset) ||
		   (y >= xyOffset + this->params.patchSize) ||
		   (x < xyOffset) ||		   
		   (x >= xyOffset + this->params.patchSize))
		{
			for(int channel = 0; channel <  this->imgShape.nBands; channel++, iElemLarge++, pixelAddr++)
			{
				patch[pixelAddr] = (float) this->dataPts[patchID][iElemLarge];
			}
		}
		else
		{
			for(int channel = 0; channel <  this->imgShape.nBands; channel++, iElemSmall++, pixelAddr++)
			{
				patch[pixelAddr] = (float) this->dataPts[patchID][iElemSmall];
			}
		}
	}

	return patch;
}
//*/

/*
CFloatImage SuperRes::getLowResLargePatch(int patchID)
{
	CShape patchShape(this->params.largePatchSize, this->params.largePatchSize, this->imgShape.nBands);
	CFloatImage patch(patchShape);

	int patchVecDims = this->params.largePatchSize * this->params.largePatchSize * this->imgShape.nBands;

	for(int iElem = 0; iElem < patchVecDims; iElem++)
	{
		patch[iElem] = (float) this->dataPts[patchID][iElem];
	}

	return patch;
}
//*/

CFloatImage SuperRes::getLowResPatch(int patchID, int patchSize)
{
	ENSURE(patchSize <= this->params.largePatchSize);
	ENSURE((this->params.largePatchSize - patchSize) % 2 == 0);
	CFloatImage largePatch = getLowResLargePatch(patchID);

	int xyOffset = (this->params.largePatchSize - patchSize) / 2;

	return ImageProcessing::CropImage(largePatch, xyOffset, xyOffset,
									  patchSize, patchSize);
}

//todo - speed up
CFloatImage SuperRes::getHiResPatch(int patchID)
{
	CFloatImage largePatch = getHiResLargePatch(patchID);
	int xyOffset = (this->params.largePatchSize - this->params.patchSize) / 2;
	return ImageProcessing::CropImage(largePatch, xyOffset, xyOffset,
									  this->params.patchSize, this->params.patchSize);
}

CFloatImage SuperRes::getLowResPatch(int patchID)
{
	CFloatImage largePatch = getLowResLargePatch(patchID);
	int xyOffset = (this->params.largePatchSize - this->params.patchSize) / 2;
	return ImageProcessing::CropImage(largePatch, xyOffset, xyOffset,
									  this->params.patchSize, this->params.patchSize);
}


void SuperRes::getPatchStatsIncr(int patchID, float *meanIncr, float *stdDevIncr)
{
	int searchVecDim = this->params.largePatchSize * this->params.largePatchSize * this->imgShape.nBands;

	int freqDataOffset = 2 * searchVecDim;
	for(int channel = 0; channel < this->imgShape.nBands; channel++)
	{
		meanIncr[channel]   = (float) this->dataPts[patchID][freqDataOffset + channel];
		stdDevIncr[channel] = (float) this->dataPts[patchID][freqDataOffset + this->imgShape.nBands + channel];		
	}
}


CFloatImage SuperRes::GetCroppedImg(CFloatImage srcImg, int scale)
{
	CShape srcShape = srcImg.Shape();

	CShape upSampleShape;
	upSampleShape.width = (int) (srcShape.width / (float) scale);
	upSampleShape.width = upSampleShape.width * scale;

	upSampleShape.height = (int) (srcShape.height / (float) scale);
	upSampleShape.height = upSampleShape.height * scale;


	if((upSampleShape.width != srcShape.width) || (upSampleShape.height != srcShape.height))
	{
		return ImageProcessing::CropImage(srcImg, 0, 0, upSampleShape.width, upSampleShape.height);
	}
	else
	{
		return srcImg;
	}
}


void SuperRes::getPatchLoc(int patchID, int &x, int &y, int &iData)
{
	int searchVecDim = this->params.largePatchSize * this->params.largePatchSize * this->imgShape.nBands;
	int freqDataOffset = 2 * searchVecDim + 2 * this->imgShape.nBands;

	iData = (int) this->dataPts[patchID][freqDataOffset + 0];
	x     = (int) this->dataPts[patchID][freqDataOffset + 1];
	y     = (int) this->dataPts[patchID][freqDataOffset + 2];
}
