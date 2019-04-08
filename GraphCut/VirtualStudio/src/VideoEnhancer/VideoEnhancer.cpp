#include "VideoEnhancer.h"

VideoEnhancer::VideoEnhancer(VideoEnhancerParams videoEnhancerParams)
{
	this->params = videoEnhancerParams;
	this->dataBands = 0;
	this->searchVecDim = 0;

	ENSURE(this->params.sPatchOverlap == 1);
	ENSURE(this->params.lPatchSize >= this->params.sPatchSize);
	ENSURE(this->params.lPatchSize  % 2 == 1);
	ENSURE(this->params.sPatchSize  % 2 == 1);

	this->searchTree = NULL;
	this->dataPts = NULL;
	this->dataPtsCount = 0;
}


VideoEnhancer::~VideoEnhancer()
{
	ENSURE(this->searchTree == NULL);
	ENSURE(this->dataPts == NULL);	
}


void VideoEnhancer::Enhance(vector<CFloatImage> &trainingImgs, CFloatImage &targetImg)
{
	ENSURE(this->params.enhancementMode == EM_SUPER_RES);

	this->dataBands = targetImg.Shape().nBands;
	this->searchVecDim = this->params.lPatchSize * 
		                 this->params.lPatchSize * 
					     this->dataBands;
	setupMatchKernel();

	createSuperResTrainData(trainingImgs);

	targetImg = GetCroppedImg(targetImg, this->params.superResParams.downSampleScale);
	this->videoFrameOrg = createLowResImg(targetImg);	
	//this->videoFrameOrg = ImageProcessing::BilateralFilter(targetImg, 3.0f, 0.1f, 5);
	this->videoFrame = createHighFreqImg(this->videoFrameOrg);

	initGlobalVars();

	vector<int> trainingIndices;
	for(int iTrain = 0; iTrain < (int) this->aImgs.size(); iTrain++)
	{
		trainingIndices.push_back(iTrain);
	}
	generateLabels(0, 0, 
				   this->frameLabelsShape.width, this->frameLabelsShape.height,
				   trainingIndices);

	freeGlobalVars();
}


void VideoEnhancer::initGlobalVars()
{
	this->frameImgShape = this->videoFrame.Shape();	

	int patchOffset = this->params.sPatchSize - this->params.sPatchOverlap;
	ENSURE(patchOffset >= 1);
	
	this->frameLabelsShape.width  = ((this->frameImgShape.width  - 1) / patchOffset) + 1;
	this->frameLabelsShape.height = ((this->frameImgShape.height - 1) / patchOffset) + 1;
	this->frameLabelsShape.nBands = 1;

	this->frameLabels.ReAllocate(this->frameLabelsShape);
	this->frameDataCosts.ReAllocate(this->frameLabelsShape);

	uint nodeAddr = 0;
	for(int y = 0; y < this->frameLabelsShape.height; y++)
	for(int x = 0; x < this->frameLabelsShape.width; x++, nodeAddr++)
	{
		this->frameLabels[nodeAddr] = new Label[this->params.maxLabels];
		this->frameDataCosts[nodeAddr] = new ANNdist[this->params.maxLabels];
	}
}


void VideoEnhancer::freeGlobalVars()
{
	uint nodeAddr = 0;
	for(int y = 0; y < this->frameLabelsShape.height; y++)
	for(int x = 0; x < this->frameLabelsShape.width; x++, nodeAddr++)
	{
		delete [] this->frameLabels[nodeAddr];
		delete [] this->frameDataCosts[nodeAddr];
	}
}
