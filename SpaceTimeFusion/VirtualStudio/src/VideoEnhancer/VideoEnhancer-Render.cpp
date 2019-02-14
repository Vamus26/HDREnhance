#include "VideoEnhancer.h"




void VideoEnhancer::generateResults()
{
	CFloatImage synImg(this->frameImgShape);
	synImg.ClearPixels();

	int sPatchHW = this->params.sPatchSize / 2;
	int lPatchHW = this->params.lPatchSize / 2;

	int patchShift = (this->params.sPatchSize - this->params.sPatchOverlap);
	uint nodeAddr = 0;
	for(int nodeY = 0, y = 0; nodeY < this->frameLabelsShape.height; nodeY++, y += patchShift)
	for(int nodeX = 0, x = 0; nodeX < this->frameLabelsShape.width;  nodeX++, x += patchShift, nodeAddr++)
	{
		Label currLabel = this->frameLabels[nodeAddr][0];

		//CFloatImage hiResPatch = getHiResPatch(currLabel);
		CFloatImage hiResPatch;

		/*

			for(int channel = 0; channel < this->imgShape.nBands; channel++)
			{
				if(this->params.normalizeMeanStdDev == false)
				{
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
			*/

		uint patchPixelAddr = 0;		
		for(int yy = y - sPatchHW; yy <= y + sPatchHW; yy++)
		for(int xx = x - sPatchHW; xx <= x + sPatchHW; xx++)
		{
			if(this->frameImgShape.InBounds(xx, yy))
			{
				for(int channel = 0; channel < this->dataBands; channel++, patchPixelAddr++)
				{
					synImg.Pixel(xx, yy, channel) = hiResPatch[patchPixelAddr];
				}
			}
		}
	}

	if(this->params.enhancementMode == EM_SUPER_RES)
	{
		CFloatImage freqAdded = synImg.Clone();
		ImageProcessing::Operate(freqAdded, freqAdded, 0.5f, ImageProcessing::OP_ADD);
		ImageIO::WriteFile(freqAdded, "freq-" + this->params.outPrefix + ".tga");

		ImageProcessing::Operate(synImg, synImg, this->videoFrameOrg, ImageProcessing::OP_ADD);
	}


	ImageIO::WriteFile(synImg, "result-" + this->params.outPrefix + ".tga");
}
