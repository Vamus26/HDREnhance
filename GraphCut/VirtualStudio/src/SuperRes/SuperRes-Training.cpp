#include "SuperRes.h"
#include "ImageIO.h"

void SuperRes::Train(vector<CFloatImage> &aImgs, vector<CFloatImage> &aPrimeImgs)
{
	ENSURE(aImgs.size() == aPrimeImgs.size());
	ENSURE(aImgs.size() > 0);

	this->imgShape = aImgs[0].Shape();

	setupMatchKernel();

	for(uint i = 0; i < aImgs.size(); i++)
	{
		this->trainingShapes.push_back(aImgs[i].Shape());
	}

	createSearchTree(aImgs, aPrimeImgs);
}

void SuperRes::createSearchTree(vector<CFloatImage> &aImgs, vector<CFloatImage> &aPrimeImgs)
{
	int sampleFreq = 2;

	ENSURE(this->params.patchSize % 2);
	ENSURE(this->params.largePatchSize % 2);

	int totalPatchCount = 0;
	for(uint iData = 0; iData < aImgs.size(); iData++)
	{
		CShape currImgShape = aImgs[iData].Shape();
		ENSURE(currImgShape == aPrimeImgs[iData].Shape());

		ENSURE(currImgShape.nBands == this->imgShape.nBands);
		int currPatchCount = (currImgShape.width / sampleFreq) + 1;
		currPatchCount *= (currImgShape.height / sampleFreq) + 1;
		totalPatchCount += currPatchCount;
	}

	int searchVecDim = this->params.largePatchSize * 
		               this->params.largePatchSize * 
					   this->imgShape.nBands;

	this->dataPts = annAllocPts(totalPatchCount, (searchVecDim * 2) + (this->imgShape.nBands * 2) + 3);
	ENSURE(this->dataPts != NULL);

	float *aMean   = new float[this->imgShape.nBands];
	float *aStdDev = new float[this->imgShape.nBands];
	float *aPrimeMean    = new float[this->imgShape.nBands];
	float *aPrimeStdDev  = new float[this->imgShape.nBands];

	int iSearchVec = 0;
	int largePatchHalfWidth = this->params.largePatchSize / 2;
	for(uint iData = 0; iData < aImgs.size(); iData++)
	{
		CFloatImage aImg = aImgs[iData];
		CFloatImage aPrimeImg = aPrimeImgs[iData];
		imgShape = aImg.Shape();

		//bug
		for(int y = 0; y < imgShape.height; y += sampleFreq)
		for(int x = 0; x < imgShape.width; x += sampleFreq, iSearchVec++)
		//for(int y = largePatchHalfWidth; y < imgShape.height - largePatchHalfWidth; y++)
		//for(int x = largePatchHalfWidth; x < imgShape.width - largePatchHalfWidth; x++, iSearchVec++)
		{
			CFloatImage aPatch = ImageProcessing::CropImageUsingReflection(aImg, 
																			 x - largePatchHalfWidth, 
																			 y - largePatchHalfWidth, 
																			 this->params.largePatchSize, this->params.largePatchSize);

			CFloatImage aPrimePatch = ImageProcessing::CropImageUsingReflection(aPrimeImg,
																				 x - largePatchHalfWidth, 
																				 y - largePatchHalfWidth, 
																				 this->params.largePatchSize, this->params.largePatchSize);

			for(int channel = 0; channel < this->imgShape.nBands; channel++)
			{
				if(this->params.normalizeMeanStdDev == false)
				{
					//float channelMean = ImageProcessing::GetMean(aPatch, channel);
					//channelMean = abs(channelMean) + SuperRes_MinMean;
					//ImageProcessing::Operate(aPatch, aPatch, channelMean, ImageProcessing::OP_DIV, channel);
					//ImageProcessing::Operate(aPrimePatch, aPrimePatch, channelMean, ImageProcessing::OP_DIV, channel);

					float channelMean, channelStdDev;
					ImageProcessing::GetMeanAndDev(aPatch, channel, channelMean, channelStdDev);
					ImageProcessing::Operate(aPatch, aPatch, channelStdDev + SuperRes_MatchEpsilon, ImageProcessing::OP_DIV, channel);
					ImageProcessing::Operate(aPrimePatch, aPrimePatch, channelStdDev + SuperRes_RenderEpsilon, ImageProcessing::OP_DIV, channel);
				}
				else
				{
					ImageProcessing::GetMeanAndDev(aPatch, channel, aMean[channel], aStdDev[channel]);
					ImageProcessing::GetMeanAndDev(aPrimePatch, channel, aPrimeMean[channel], aPrimeStdDev[channel]);
					//check - remap aPrimePatch to same space as aPatch?
					ImageProcessing::RemapMeanAndDev(aPatch, channel, 0.0f, 1.0f);
					ImageProcessing::RemapMeanAndDev(aPrimePatch, channel, 0.0f, 1.0f);
				}
			}

			createLowResLargePatch(aPatch, dataPts[iSearchVec]);
			for(int iElem = 0; iElem < searchVecDim; iElem++)
			{
				//dataPts[iSearchVec][iElem] = (ANNcoord) (this->matchKernel[iElem] * aPatch[iElem]);
				dataPts[iSearchVec][iElem + searchVecDim] = (ANNcoord) (this->matchKernel[iElem] * aPrimePatch[iElem]);
			}

			int freqDataOffset = 2 * searchVecDim;
			for(int channel = 0; channel < this->imgShape.nBands; channel++)
			{
				dataPts[iSearchVec][freqDataOffset + channel] = (ANNcoord) (aPrimeMean[channel] - aMean[channel]);
				dataPts[iSearchVec][freqDataOffset + imgShape.nBands + channel] = (ANNcoord) (aPrimeStdDev[channel] - aStdDev[channel]);
			}

			freqDataOffset = (2 * searchVecDim) + (2 * this->imgShape.nBands);
			dataPts[iSearchVec][freqDataOffset + 0] = iData;
			dataPts[iSearchVec][freqDataOffset + 1] = x;
			dataPts[iSearchVec][freqDataOffset + 2] = y;
		}
	}		

	printf("Bulding KD Tree with %i patches...\n", totalPatchCount);
	this->searchTree = new ANNkd_tree(dataPts, totalPatchCount, searchVecDim);
	//this->searchTree2 = new ANNkd_tree(dataPts, totalPatchCount, 
	//	this->params.patchSize * this->params.patchSize * this->imgShape.nBands);
	printf("  Done.\n");

	delete aPrimeMean;
	delete aMean;
	delete aStdDev;
	delete aPrimeStdDev;
}

void SuperRes::TrainSuperRes(vector<CFloatImage> &trainingImages)
{	
	ENSURE(this->params.downSampleScale >= 1);

	for(uint iData = 0; iData < trainingImages.size(); iData++)
	{
		CFloatImage superResImg = trainingImages[iData];
		CFloatImage lowResImg = createLowResImg(superResImg);
		ENSURE(lowResImg.Shape() == superResImg.Shape());

		this->aImgsOrg.push_back(lowResImg);
		this->aPrimeImgsOrg.push_back(superResImg);

		CFloatImage superResHighFreq = createHighFreqImg(superResImg);
		CFloatImage lowResHighFreq   = createHighFreqImg(lowResImg);

		aImgs.push_back(lowResHighFreq);
		aPrimeImgs.push_back(superResHighFreq);

		//////bug
		//CFloatImage test1 = lowResHighFreq.Clone();
		//ImageProcessing::Operate(test1, test1, 0.5f, ImageProcessing::OP_ADD);
		//ImageIO::WriteFile(test1 ,"1.tga");

		//////bug
		//CFloatImage test2 = superResHighFreq.Clone();
		//ImageProcessing::Operate(test2, test2, 0.5f, ImageProcessing::OP_ADD);
		//ImageIO::WriteFile(test2, "2.tga");

		//ImageIO::WriteFile(lowResImg ,"3.tga");
		//exit(0);
	}

	this->params.normalizeMeanStdDev = false;
	Train(aImgs, aPrimeImgs);
}

void SuperRes::TrainFilter(vector<CFloatImage> &aImgs, vector<CFloatImage> &aPrimeImgs)
{
	for(uint iData = 0; iData < aImgs.size(); iData++)
	{
		if(aImgs[iData].Shape().nBands != 1)
		{
			aImgs[iData] = ColorHelper::RGBtoLuminance(aImgs[iData]);
			aPrimeImgs[iData] = ColorHelper::RGBtoLuminance(aPrimeImgs[iData]);
		}
	}

	this->params.normalizeMeanStdDev = true;
	Train(aImgs, aPrimeImgs);
}


CFloatImage SuperRes::createLowResImg(CFloatImage superResImg)
{
	//check - solve for kernelsize
	int kernelSize = (int) max(5.0, 3.0 * this->params.blurKernelStdDev);
	kernelSize += !(kernelSize % 2);

	CFloatImage superResImgBlur = ImageProcessing::GaussianBlur(superResImg, kernelSize, 
																this->params.blurKernelStdDev);	

	CFloatImage lowResImgSmall;
	ImageProcessing::ScaleImage(lowResImgSmall, superResImgBlur, 1.0f / this->params.downSampleScale, false);	

	CFloatImage lowResImg;
	ImageProcessing::ScaleImage(lowResImg, lowResImgSmall, 1.0f * this->params.downSampleScale, false);

	//bug 
	//lowResImg = ImageProcessing::BilateralFilter(lowResImg, 3.0f, 0.2f, 3);

	ENSURE(lowResImg.Shape() == superResImg.Shape());

	return lowResImg;	
}

CFloatImage SuperRes::createHighFreqImg(CFloatImage srcImg)
{
	int kernelSize = (int) max(5.0, 3.0 * this->params.dogKernelStdDev);
	kernelSize += !(kernelSize % 2);
	CFloatImage srcImgBlur = ImageProcessing::GaussianBlur(srcImg, kernelSize, 
														   this->params.dogKernelStdDev);

	CFloatImage highFreqImg(srcImg.Shape());
	ImageProcessing::Operate(highFreqImg, srcImg, srcImgBlur, ImageProcessing::OP_SUB);

	return highFreqImg;
}
