#include "VideoEnhancer.h"

CFloatImage VideoEnhancer::createLowResImg(CFloatImage hiResImg)
{
	int kernelSize = (int) max(5.0, 3.0 * this->params.superResParams.blurKernelStdDev);
	kernelSize += !(kernelSize % 2);
	CFloatImage hiResImgBlur = ImageProcessing::GaussianBlur(hiResImg, kernelSize, 
														 	 this->params.superResParams.blurKernelStdDev);	

	CFloatImage lowResImgSmall;
	ImageProcessing::ScaleImage(lowResImgSmall, hiResImgBlur, 1.0f / this->params.superResParams.downSampleScale, false);	

	CFloatImage lowResImg;
	ImageProcessing::ScaleImage(lowResImg, lowResImgSmall, 1.0f * this->params.superResParams.downSampleScale, false);

	ENSURE(lowResImg.Shape() == hiResImg.Shape());

	return lowResImg;	
}

CFloatImage VideoEnhancer::createHighFreqImg(CFloatImage srcImg)
{
	int kernelSize = (int) max(5.0, 3.0 * this->params.superResParams.dogKernelStdDev);
	kernelSize += !(kernelSize % 2);
	CFloatImage srcImgBlur = ImageProcessing::GaussianBlur(srcImg, kernelSize, 
														   this->params.superResParams.dogKernelStdDev);

	CFloatImage highFreqImg(srcImg.Shape());
	ImageProcessing::Operate(highFreqImg, srcImg, srcImgBlur, ImageProcessing::OP_SUB);

	return highFreqImg;
}



CFloatImage VideoEnhancer::GetCroppedImg(CFloatImage srcImg, int scale)
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