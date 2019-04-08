#include "VideoEnhancer.h"
#include "ImageProcessing.h"

void VideoEnhancer::createSearchTree(const vector<int> &trainingIndices)
{
	ENSURE(this->params.trainingDataSF > 0);
	ENSURE(this->dataBands > 0);
	ENSURE(this->aImgs.size() < USHRT_MAX);

	printf("Extracting patches for KD Tree...\n");
	fflush(stdout);

	//check - more accurate memory alloc
	int totalPatchCount = 0;
	for(uint iTrain = 0; iTrain < trainingIndices.size(); iTrain++)
	{
		int trainingIndex = trainingIndices[iTrain];

		CShape currImgShape = this->aImgs[trainingIndex].Shape();
		ENSURE(currImgShape.nBands == this->dataBands);
		ENSURE(currImgShape.width < USHRT_MAX);
		ENSURE(currImgShape.height < USHRT_MAX);

		int currPatchCount = (currImgShape.width - this->params.lPatchSize + 1) / this->params.trainingDataSF + 1;
		currPatchCount *= ((currImgShape.height - this->params.lPatchSize + 1) / this->params.trainingDataSF + 1);
		totalPatchCount += currPatchCount;
	}

	this->dataPts = annAllocPts(totalPatchCount, this->searchVecDim + getAuxElemCount());
	ENSURE(this->dataPts != NULL);

	this->dataPtsCount = 0;
	int lPatchHW = this->params.lPatchSize / 2;
	for(uint iTrain = 0; iTrain < trainingIndices.size(); iTrain++)
	{
		int trainingIndex = trainingIndices[iTrain];

		CFloatImage &aImg = this->aImgs[trainingIndex];
		CByteImage &aMask = this->trainMasks[trainingIndex];
		CShape currImgShape = aImg.Shape();

		for(int y = lPatchHW; y < currImgShape.height - lPatchHW; y += this->params.trainingDataSF)
		for(int x = lPatchHW; x < currImgShape.width  - lPatchHW; x += this->params.trainingDataSF)
		{
			bool patchViolatesMask = false;
			CFloatImage aPatch = ImageProcessing::CropImage(aImg, aMask, patchViolatesMask, 
														    x - lPatchHW, 
															y - lPatchHW, 
															this->params.lPatchSize, this->params.lPatchSize);
			if(patchViolatesMask == false)
			{
				Label dataPtLabel(iTrain, x, y);
				createDataPtFromPatch(dataPts[this->dataPtsCount], aPatch, dataPtLabel);
				this->dataPtsCount++;
			}			
		}
	}

	ENSURE(this->dataPtsCount > 0);

	printf("Bulding KD Tree with %i patches...\n", this->dataPtsCount);
	fflush(stdout);
	this->searchTree = new ANNkd_tree(dataPts, this->dataPtsCount, this->searchVecDim);
	printf("  Done.\n");
	fflush(stdout);
}

void VideoEnhancer::destroySearchTree()
{
	ENSURE(this->dataPts != NULL);
	ENSURE(this->searchTree != NULL);

	annDeallocPts(this->dataPts);
	delete this->searchTree;

	this->dataPts = NULL;
	this->searchTree = NULL;
	this->dataPtsCount = 0;
}

void VideoEnhancer::createSuperResTrainData(vector<CFloatImage> &trainingImages)
{	
	ENSURE(this->params.superResParams.downSampleScale >= 1);	

	for(uint iData = 0; iData < trainingImages.size(); iData++)
	{
		CFloatImage superResImg = trainingImages[iData];
		superResImg = VideoEnhancer::GetCroppedImg(superResImg, this->params.superResParams.downSampleScale);

		CFloatImage lowResImg = createLowResImg(superResImg);
		ENSURE(lowResImg.Shape() == superResImg.Shape());
		ENSURE(lowResImg.Shape().nBands == this->dataBands);

		this->aImgsOrg.push_back(lowResImg);
		this->aPrimeImgsOrg.push_back(superResImg);

		CFloatImage superResHighFreq = createHighFreqImg(superResImg);
		CFloatImage lowResHighFreq   = createHighFreqImg(lowResImg);

		this->aImgs.push_back(lowResHighFreq);
		this->aPrimeImgs.push_back(superResHighFreq);

		CShape maskShape = this->aImgs[iData].Shape();
		maskShape.nBands = 1;
		CByteImage trainMask(maskShape);
		ImageProcessing::Operate(trainMask, trainMask, MASK_VALID, ImageProcessing::OP_SET);
		this->trainMasks.push_back(trainMask);
	}

	ENSURE(this->params.enhancementMode == EM_SUPER_RES);
}

void VideoEnhancer::createFilterTrainData(vector<CFloatImage> &unfilteredImgs, vector<CFloatImage> &filteredImgs)
{
	for(uint iData = 0; iData < aImgs.size(); iData++)
	{
		this->aImgsOrg.push_back(unfilteredImgs[iData]);
		this->aPrimeImgs.push_back(filteredImgs[iData]);

		if(aImgs[iData].Shape().nBands != 1)
		{
			this->aImgs.push_back(ColorHelper::RGBtoLuminance(unfilteredImgs[iData]));
			this->aPrimeImgs.push_back(ColorHelper::RGBtoLuminance(filteredImgs[iData]));
		}
		else
		{
			this->aImgs.push_back(unfilteredImgs[iData].Clone());
			this->aPrimeImgs.push_back(filteredImgs[iData].Clone());
		}

		CShape maskShape = this->aImgs[iData].Shape();
		maskShape.nBands = 1;
		CByteImage trainMask(maskShape);
		ImageProcessing::Operate(trainMask, trainMask, MASK_VALID, ImageProcessing::OP_SET);
		this->trainMasks.push_back(trainMask);
	}

	ENSURE(this->params.enhancementMode == EM_FILTER);
}
