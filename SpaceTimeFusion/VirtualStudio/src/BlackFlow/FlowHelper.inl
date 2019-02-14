#ifndef __FLOW_HELPER_INL__
#define __FLOW_HELPER_INL__

template <class T>
void FlowHelper::WarpImage(CImageOf<T> &warppedImg,  CByteImage &warppedMask, 
						   CImageOf<T> &sourceImage, 
						   CFloatImage &flowField,   
						   bool useSampling,
						   T defaultVal)
{
	CByteImage dummyMask;
	FlowHelper::WarpImage(warppedImg, warppedMask, 
						  sourceImage, dummyMask, false,
						  flowField, useSampling, defaultVal);
}

template <class T>
void FlowHelper::WarpImage(CImageOf<T> &warppedImg,  CByteImage &warppedMask, 
						   CImageOf<T> &sourceImage, CByteImage &sourceMask, bool useSourceMask,
						   CFloatImage &flowField,   
						   bool useSampling,
						   T defaultVal)
{
	CShape warpImgShape   = warppedImg.Shape();
	CShape sourceImgShape = sourceImage.Shape();
	CShape flowFieldShape = flowField.Shape();

	INSIST(warpImgShape.nBands == sourceImgShape.nBands);
	INSIST(warpImgShape.SameIgnoringNBands(warppedMask.Shape()));
	if(useSourceMask)
	{
		INSIST(sourceImgShape.SameIgnoringNBands(sourceMask.Shape()));
		INSIST(sourceMask.Shape().nBands == 1);
	}
	INSIST(warpImgShape.SameIgnoringNBands(flowFieldShape));
	INSIST(flowFieldShape.nBands == 3);
	INSIST(warppedMask.Shape().nBands == 1);

	warppedMask.ClearPixels();
	ImageProcessing::Operate(warppedImg, warppedImg, defaultVal, ImageProcessing::OP_SET);

	uint flowIndex  = 0;
	uint pixelIndex = 0;
	uint maskIndex  = 0;
	for(int y = 0; y < warpImgShape.height; y++)
	for(int x = 0; x < warpImgShape.width;  x++, pixelIndex += warpImgShape.nBands, flowIndex += flowFieldShape.nBands, maskIndex++)
	{	
		float u = flowField[flowIndex + 0];
		float v = flowField[flowIndex + 1];

		float xx = x - u;
		float yy = y - v;

		if(sourceImgShape.InBounds(xx, yy) == false) continue;
		int integerXX = NEAREST_INT(xx);
		int integerYY = NEAREST_INT(yy);				
		if((useSourceMask) && (sourceMask.Pixel(integerXX, integerYY, 0) == MASK_INVALID))
		{
			continue;
		}
		
		warppedMask[maskIndex] = MASK_VALID;
		if(useSampling)
			ImageProcessing::MitchellInterlopation(warppedImg, pixelIndex, sourceImage, xx, yy);
		else
		{			
			uint sourcePixelIndex = sourceMask.PixelIndex(integerXX, integerXX, 0);				
			for(int iBand = 0; iBand < warpImgShape.nBands; iBand++)
			{
				warppedImg[pixelIndex + iBand] = sourceImage[sourcePixelIndex + iBand];
			}
		}
	}
}

#endif
