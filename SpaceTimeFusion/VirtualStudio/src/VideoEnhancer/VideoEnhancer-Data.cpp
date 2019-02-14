#include "VideoEnhancer.h"

int VideoEnhancer::getAuxElemCount()
{
	return 3 + (this->dataBands * 2);
}

void VideoEnhancer::createDataPtFromPatch(ANNpoint dataPt, CFloatImage &patch, const Label &dataPtLabel)
{
	ENSURE(this->dataBands <= 3);
	float means[3]; 
	float stdDevs[3]; 

	if(this->params.enhancementMode == EM_SUPER_RES)
	{
		for(int channel = 0; channel < this->dataBands; channel++)
		{
			ImageProcessing::GetMeanAndDev(patch, channel, means[channel], stdDevs[channel]);
			ImageProcessing::Operate(patch, patch, means[channel] + this->params.matchEpsilon, ImageProcessing::OP_DIV, channel);

			dataPt[this->searchVecDim + channel] = means[channel];
			dataPt[this->searchVecDim + this->dataBands + channel] = stdDevs[channel];
		}
	}
	else
	{
		REPORT_FAILURE("Not yet implemented");
		ENSURE(this->params.enhancementMode == EM_FILTER);
		for(int channel = 0; channel < this->dataBands; channel++)
		{
			ImageProcessing::GetMeanAndDev(patch, channel, means[channel], stdDevs[channel]);
			ImageProcessing::Operate(patch, patch, means[channel], ImageProcessing::OP_SUB, channel);
			ImageProcessing::Operate(patch, patch, stdDevs[channel] + this->params.matchEpsilon, ImageProcessing::OP_DIV, channel);

			dataPt[this->searchVecDim + channel] = means[channel];
			dataPt[this->searchVecDim + this->dataBands + channel] = stdDevs[channel];
		}
	}

	for(int iElem = 0; iElem < this->searchVecDim; iElem++)
	{
		dataPt[iElem] = (ANNcoord) (this->matchKernel[iElem] * patch[iElem]);
	}

	int labelOffset = this->searchVecDim + (2 * this->dataBands);
	dataPt[labelOffset + 0] = (ANNcoord) dataPtLabel.iTrain;
	dataPt[labelOffset + 1] = (ANNcoord) dataPtLabel.x;
	dataPt[labelOffset + 2] = (ANNcoord) dataPtLabel.y;
}


VideoEnhancer::Label VideoEnhancer::extractLabelFromDataPt(int dataPtID)
{
	int labelOffset = this->searchVecDim + (2 * this->dataBands);
	const ANNpoint dataPt = this->dataPts[dataPtID];	
	Label label;

	label.iTrain = (unsigned short)  dataPt[labelOffset + 0];
	label.x      = (unsigned short)  dataPt[labelOffset + 1];
	label.y      = (unsigned short)  dataPt[labelOffset + 2];

	return label;
}


void VideoEnhancer::normalizePatch(CFloatImage &patch, float *means, float *stdDevs)
{
	if(this->params.enhancementMode == EM_SUPER_RES)
	{
		for(int channel = 0; channel < this->dataBands; channel++)
		{
			ImageProcessing::GetMeanAndDev(patch, channel, means[channel], stdDevs[channel]);
			ImageProcessing::Operate(patch, patch, means[channel] + this->params.matchEpsilon, ImageProcessing::OP_DIV, channel);

			dataPt[this->searchVecDim + channel] = means[channel];
			dataPt[this->searchVecDim + this->dataBands + channel] = stdDevs[channel];
		}
	}
	else
	{
		REPORT_FAILURE("Not yet implemented");
		ENSURE(this->params.enhancementMode == EM_FILTER);
		for(int channel = 0; channel < this->dataBands; channel++)
		{
			ImageProcessing::GetMeanAndDev(patch, channel, means[channel], stdDevs[channel]);
			ImageProcessing::Operate(patch, patch, means[channel], ImageProcessing::OP_SUB, channel);
			ImageProcessing::Operate(patch, patch, stdDevs[channel] + this->params.matchEpsilon, ImageProcessing::OP_DIV, channel);

			dataPt[this->searchVecDim + channel] = means[channel];
			dataPt[this->searchVecDim + this->dataBands + channel] = stdDevs[channel];
		}
	}
}

void VideoEnhancer::normalizePatches(CFloatImage &aPatch, CFloatImage &aPrimePatch, float *means, float *stdDevs)
{
	if(this->params.enhancementMode == EM_SUPER_RES)
	{
		for(int channel = 0; channel < this->dataBands; channel++)
		{
			ImageProcessing::GetMeanAndDev(aPatch, channel, means[channel], stdDevs[channel]);
			ImageProcessing::Operate(aPatch, aPatch, means[channel] + this->params.matchEpsilon, ImageProcessing::OP_DIV, channel);
			ImageProcessing::Operate(aPrimePatch, aPrimePatch, means[channel] + this->params.renderEpsilon, ImageProcessing::OP_DIV, channel);

			dataPt[this->searchVecDim + channel] = means[channel];
			dataPt[this->searchVecDim + this->dataBands + channel] = stdDevs[channel];
		}
	}
	else
	{
		REPORT_FAILURE("Not yet implemented");
		ENSURE(this->params.enhancementMode == EM_FILTER);
		for(int channel = 0; channel < this->dataBands; channel++)
		{
			ImageProcessing::GetMeanAndDev(aPatch, channel, means[channel], stdDevs[channel]);
			ImageProcessing::Operate(aPatch, aPatch, means[channel], ImageProcessing::OP_SUB, channel);
			ImageProcessing::Operate(aPatch, aPatch, stdDevs[channel] + this->params.matchEpsilon, ImageProcessing::OP_DIV, channel);

			ImageProcessing::Operate(aPrimePatch, aPrimePatch, means[channel], ImageProcessing::OP_SUB, channel);
			ImageProcessing::Operate(aPrimePatch, aPrimePatch, stdDevs[channel] + this->params.renderEpsilon, ImageProcessing::OP_DIV, channel);

			dataPt[this->searchVecDim + channel] = means[channel];
			dataPt[this->searchVecDim + this->dataBands + channel] = stdDevs[channel];
		}
	}
}

void VideoEnhancer::getAPrimePatch(Label label)
{
	CFloatImage &aImg      = this->aImgs[label.iTrain];
	CFloatImage &aPrimeImg = this->aPrimeImgs[label.iTrain];

	int lPatchHW = this->params.lPatchSize / 2;
	
	CFloatImage aPatch = ImageProcessing::CropImage(aImg,
												    label.x - lPatchHW, 
													label.y - lPatchHW, 
													this->params.lPatchSize, this->params.lPatchSize);

	CFloatImage aPrimePatch = ImageProcessing::CropImage(aPrimeImg,
												         label.x - lPatchHW, 
													     label.y - lPatchHW, 
													     this->params.lPatchSize, this->params.lPatchSize);
}