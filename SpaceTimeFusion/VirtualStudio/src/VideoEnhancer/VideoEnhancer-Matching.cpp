#include "VideoEnhancer.h"

void VideoEnhancer::setupMatchKernel()
{
	ENSURE(this->dataBands != 0);
	CShape kernelShape(this->params.lPatchSize, 
					   this->params.lPatchSize, 
					   this->dataBands);
	this->matchKernel.ReAllocate(kernelShape);

	int lPatchHW = this->params.lPatchSize / 2;

	CHECK_RANGE(this->params.matchKernelCornerWeight, 
		        FLT_EPSILON, 1.0f - FLT_EPSILON);
	float denom = (lPatchHW * lPatchHW) / 
		          log(this->params.matchKernelCornerWeight);

	int nodeAddr = 0;
	for(float x = 0; x < this->params.lPatchSize; x++)
	for(float y = 0; y < this->params.lPatchSize; y++)
	{
		float diff;
		float dist = 0.0f;
		diff = x - lPatchHW;
		dist += (diff * diff);
		diff = y - lPatchHW;
		dist += (diff * diff);

		float weight = exp(dist / denom);

		for(int channel = 0; channel < this->dataBands; channel++, nodeAddr++)
		{
			matchKernel[nodeAddr] = weight;
		}
	}
}

void VideoEnhancer::generateLabels(int startNodeX, int startNodeY, 
								   int nodeSpanW, int nodeSpanH, 
								   const vector<int> &trainingIndices)
{
	printf("Generating labels...\n");
	fflush(stdout);

	ENSURE(this->frameLabelsShape.InBounds(startNodeX, startNodeY));
	ENSURE(this->frameLabelsShape.InBounds(startNodeX + nodeSpanW - 1, startNodeY + nodeSpanH - 1));

	createSearchTree(trainingIndices);	
	ANNpoint queryPt = annAllocPt(this->searchVecDim + getAuxElemCount());
	ANNidx *nodeIDs  = new ANNidx[this->params.maxLabels];

	Label dummyLabel;

	int lPatchHW = this->params.lPatchSize / 2;
	int sPatchHW = this->params.sPatchSize / 2;

	int patchShift = (this->params.sPatchSize - this->params.sPatchOverlap);	
	for(int nodeY = startNodeY, y = patchShift * startNodeY; 
		nodeY < startNodeY + nodeSpanH; 
		nodeY++, y += patchShift)
	{
		uint nodeAddr = this->frameLabelsShape.NodeIndex(startNodeX, nodeY);

		for(int nodeX = startNodeX, x = patchShift * startNodeX; 
			nodeX < startNodeX + nodeSpanW; 
			nodeX++, x += patchShift, nodeAddr++)
		{
			CFloatImage currPatch = ImageProcessing::CropImageUsingReflection(this->videoFrame,
																			  x + sPatchHW - lPatchHW,
																			  y + sPatchHW - lPatchHW,
																			  this->params.lPatchSize, this->params.lPatchSize);

			createDataPtFromPatch(queryPt, currPatch, dummyLabel);

			this->searchTree->annkSearch(queryPt,
										 this->params.maxLabels,
										 nodeIDs,
										 this->frameDataCosts[nodeAddr],
										 this->params.annErrorBound);

			for(int iLabel = 0; iLabel < this->params.maxLabels; iLabel++)
			{
				this->frameLabels[nodeAddr][iLabel] = extractLabelFromDataPt(nodeIDs[iLabel]);
			}
		}
		printf(".");
		fflush(stdout);
	}

	delete [] nodeIDs;
	annDeallocPt(queryPt);
	destroySearchTree();

	printf("\n  Done.\n");
	fflush(stdout);
}