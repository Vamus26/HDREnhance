#include "SuperRes.h"

void SuperRes::bpIter(int iFrame)
{
	CFltPtrPtrImage &frameMsges = this->spatialMsges[iFrame];
	CFltPtrPtrImage &framePrevMsges = this->prevSpatialMsges[iFrame];

	uint nodeAddr = 0;
	for(int y = 0; y < this->labelShape.height; y++)
	for(int x = 0; x < this->labelShape.width; x++, nodeAddr++)
	for(int iNeigh = 0; iNeigh < BP_Connectivity; iNeigh++)
	{
		if(frameMsges[nodeAddr][iNeigh] != NULL)
		{
			int neighX = x + BP_DX[iNeigh];
			int neighY = y + BP_DY[iNeigh];
			ENSURE(this->labelShape.InBounds(neighX, neighY) == true);
			uint neighNodeAddr = this->labelShape.NodeIndex(neighX, neighY);
			computeSpatialMsg(nodeAddr, neighNodeAddr, iFrame, iNeigh);
		}

		//if((x == 0) && (iNeigh == 0))
		//	printf(".");
	}

	nodeAddr = 0;	
	for(int y = 0; y < this->labelShape.height; y++)
	for(int x = 0; x < this->labelShape.width; x++, nodeAddr++)
	for(int iNeigh = 0; iNeigh < BP_Connectivity; iNeigh++)
	{
		if(frameMsges[nodeAddr][iNeigh] != NULL)
		{
			for(int iElem = 0; iElem < this->params.maxLabels; iElem++)
			{
				framePrevMsges[nodeAddr][iNeigh][iElem] = frameMsges[nodeAddr][iNeigh][iElem];
			}
		}
	}

	printf(".");

	generateResults(iFrame, false);
}

void SuperRes::computeSpatialMsg(uint nodeAddr, uint neighNodeAddr, int iFrame, int neighIndex)
{
	CFltPtrPtrImage &frameMsges = this->spatialMsges[iFrame];
	CFltPtrPtrImage &framePrevMsges = this->prevSpatialMsges[iFrame];
	CIntPtrImage &frameLabels = this->labels[iFrame];
	CDlbPtrImage &frameDataCost = this->datacosts[iFrame];

	CFltPtrImage &frameMinDist = this->spatialMinDists[iFrame];
	CFltPtrImage &frameProbSum = this->spatialProbSum[iFrame];

	int *nodeLabels = frameLabels[nodeAddr];
	int *neighNodeLabels = frameLabels[neighNodeAddr];

	int neighComplementIndex = BP_NEIGH_COMPLEMENT[neighIndex];

	//speed up
	float *tempMsg = new float[this->params.maxLabels];
	double *nodeDataCost = frameDataCost[nodeAddr];
	//memcpy(targetMsg, nodeDataCost, this->params.maxLabels * sizeof(*nodeDataCost));
	for(int iElem = 0; iElem < this->params.maxLabels; iElem++)
	{
		tempMsg[iElem] = (float) nodeDataCost[iElem];
	}

	for(int iNeigh = 0; iNeigh < BP_Connectivity; iNeigh++)
	{
		float *incomingMsg = framePrevMsges[nodeAddr][iNeigh];
		if((iNeigh == neighIndex) ||
		   (incomingMsg == NULL))
		{
			continue;
		}

		for(int iElem = 0; iElem < this->params.maxLabels; iElem++)
		{
			tempMsg[iElem] *= incomingMsg[iElem];
		}
	}

	float minDist = frameMinDist[nodeAddr][neighIndex];
	float probNorm = frameProbSum[nodeAddr][neighIndex];
	float spatialProbDenom = -2.0f * this->params.spatialCostNoiseStdDev * this->params.spatialCostNoiseStdDev;

	float beliefSum = FLT_EPSILON;
	float *targetMsg = frameMsges[neighNodeAddr][neighComplementIndex];
	for(int jElem = 0; jElem < this->params.maxLabels; jElem++)
	{
		targetMsg[jElem] = 0.0f;
		int neighLabel = neighNodeLabels[jElem];
		for(int iElem = 0; iElem < this->params.maxLabels; iElem++)
		{
			int nodeLabel = nodeLabels[iElem];

			float currDist = computeSpatialSmoothnessDist(nodeLabel, neighLabel, (BoundaryType) neighIndex, nodeAddr, neighNodeAddr, iFrame);
			ENSURE(currDist >= minDist);
			float probDist = 1.0f - (minDist / currDist);
			float smoothnessProb = exp(probDist * probDist / spatialProbDenom) / probNorm;

			targetMsg[jElem] += (smoothnessProb * tempMsg[iElem]);
		}
		beliefSum += targetMsg[jElem];
	}

	ENSURE(beliefSum > 0.0f);

	for(int jElem = 0; jElem < this->params.maxLabels; jElem++)
	{
		targetMsg[jElem] /= beliefSum;
		targetMsg[jElem] += 0.000001f;
	}

	delete tempMsg;
	//uint neighNodeAddr = this->labelShape.NodeIndex(neighX, neighY);
}

int SuperRes::getLabel(int iFrame, int nodeAddr)
{
	float *tempMsg = new float[this->params.maxLabels];
	CDlbPtrImage &frameDataCost = this->datacosts[iFrame];
	CFltPtrPtrImage &framePrevMsges = this->prevSpatialMsges[iFrame];

	CIntPtrImage &frameLabels = this->labels[iFrame];

	double *nodeDataCost = frameDataCost[nodeAddr];

	for(int iElem = 0; iElem < this->params.maxLabels; iElem++)
	{
		tempMsg[iElem] = (float) nodeDataCost[iElem];
	}

	for(int iNeigh = 0; iNeigh < BP_Connectivity; iNeigh++)
	{
		if(framePrevMsges[nodeAddr][iNeigh] != NULL)
		{
			float *incomingMsg = framePrevMsges[nodeAddr][iNeigh];
			for(int iElem = 0; iElem < this->params.maxLabels; iElem++)
			{
				tempMsg[iElem] *= incomingMsg[iElem];
			}
		}
	}

	int bestLabelIndex = -1;
	float bestLabelProb = 0.0f;

	for(int iElem = 0; iElem < this->params.maxLabels; iElem++)
	{
		if(tempMsg[iElem] > bestLabelProb)
		{
			bestLabelIndex = iElem;
			bestLabelProb = tempMsg[iElem];
		}		 
	}

	delete tempMsg;

	return frameLabels[nodeAddr][bestLabelIndex];
}