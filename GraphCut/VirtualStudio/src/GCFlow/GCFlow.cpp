#include "GCFlow.h"
#include "ColorHelper.h"
#include "ImageIO.h"

GCFlow::GCFlow(CFloatImage currImage, CFloatImage nextImage)
{
	this->currImg = ColorHelper::RGBtoLuminance(currImage);
	this->nextImg = ColorHelper::RGBtoLuminance(nextImage);
	ENSURE(this->currImg.Shape() == this->nextImg.Shape());

	this->imgShape = this->currImg.Shape();
	this->maskShape = this->imgShape;
	this->maskShape.nBands = 1;

	this->graphNodes.ReAllocate(this->maskShape);
	this->labelImage.ReAllocate(this->maskShape);
	uint nodeAddr = 0;
	IntPair defaultLabel(0, 0);
	for(int y = 0; y < this->maskShape.height; y++)
	for(int x = 0; x < this->maskShape.width; x++, nodeAddr++)
	{
		//bug
		this->labelImage[nodeAddr] = defaultLabel;


		//if(x <  this->maskShape.width - 1)
		//	this->labelImage[nodeAddr] = IntPair(1, 0);
		//else
		//	this->labelImage[nodeAddr] = IntPair(1, 0);
	}

	//ImageIO::WriteFile(this->currImg, "1.tga");
	//ImageIO::WriteFile(this->nextImg, "2.tga");

	//this->params = GCFlow::GCFlowParams::Default();
	//PRINT_DOUBLE(computeCurrEnergy());
	//exit(0);
}

void GCFlow::ComputeFlow(GCFlowParams gcFlowParams, CFloatImage &uImg, CFloatImage &vImg)
{
	//bug blur the input images by the right amount

	this->params = gcFlowParams;

	int staticIterCount = 0;
	int maxStaticIterCount = (this->params.endV - this->params.startV + 1);
	maxStaticIterCount *= (this->params.endU - this->params.startU + 1);

	for(int iIter = 0; iIter < this->params.maxIterations; iIter++)
	{
		for(int labelV = this->params.startV; labelV <= this->params.endV; labelV++)
		for(int labelU = this->params.startU; labelU <= this->params.endU; labelU++)
		{
			if(staticIterCount == maxStaticIterCount)
				continue;

			//bug
			if(expand(IntPair(labelU, labelV)) == true)
			//if(expand(std::make_pair(0, 0)) == true)
			{
				PRINT_INT(labelU);
				PRINT_INT(labelV);
				//exit(0);
				staticIterCount++;
				printf("< ");
			}
			else
			{
				staticIterCount = 0;
				printf("> ");
			}
		}
		printf("\n");
	}	

	uImg.ReAllocate(this->maskShape);
	vImg.ReAllocate(this->maskShape);
	uint nodeAddr = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	for(int x = 0; x < this->maskShape.width; x++, nodeAddr++)
	{
		uImg[nodeAddr] = (float) this->labelImage[nodeAddr].first;
		vImg[nodeAddr] = (float) this->labelImage[nodeAddr].second;
	}
}


void gc_error_function(char *msg) 
{
	REPORT_FAILURE("Graphcut error: %s\n", msg);  
}
#define INVARIANT_NODE ((Energy::Var) 0)

bool GCFlow::expand(IntPair newLabel)
{
	Energy *efunc = new Energy(gc_error_function);

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	for(int x = 0; x < this->maskShape.width; x++, nodeAddr++, pixelAddr += this->imgShape.nBands)
	{
		IntPair nodeLabel = this->labelImage[nodeAddr];
		if(nodeLabel == newLabel)
		{
			this->graphNodes[nodeAddr] = INVARIANT_NODE;
			efunc->add_constant(dataCost(x, y, nodeLabel));
		}
		else
		{
			this->graphNodes[nodeAddr] = efunc->add_variable();
			efunc->add_term1(this->graphNodes[nodeAddr],
							 dataCost(x, y, nodeLabel),
							 dataCost(x, y, newLabel));
		}
	}

	nodeAddr  = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	for(int x = 0; x < this->maskShape.width;  x++, nodeAddr++)
	{
		IntPair nodeLabel = this->labelImage[nodeAddr];			
		Energy::Var nodeVar = this->graphNodes[nodeAddr];

		for(int iEdge = 0; iEdge < GCFLOW_Connectivity; iEdge++)
		{
			int neighX = x + GCFLOW_DX[iEdge];
			int neighY = y + GCFLOW_DY[iEdge];
			if(this->maskShape.InBounds(neighX, neighY) == false) continue;

			uint neighNodeAddr       = this->maskShape.NodeIndex(neighX, neighY, 0);
			IntPair neighNodeLabel   = this->labelImage[neighNodeAddr];				
			Energy::Var neighNodeVar = this->graphNodes[neighNodeAddr];

			float e00 = 0.0f, e01 = 0.0f, e10 = 0.0f, e11 = 0.0f;

			if(nodeVar != INVARIANT_NODE)
				e01 = edgeCost(nodeLabel, newLabel);

			if(neighNodeVar != INVARIANT_NODE)
				e10 = edgeCost(newLabel, neighNodeLabel);

			if((nodeVar != INVARIANT_NODE) && (neighNodeVar != INVARIANT_NODE))
				e00 = edgeCost(nodeLabel, neighNodeLabel);					

			if(nodeVar != INVARIANT_NODE)
			{
				if(neighNodeVar != INVARIANT_NODE)
					efunc->add_term2(nodeVar, neighNodeVar, e00, e01, e10, e11);
				else
					efunc->add_term1(nodeVar, e01, e11);
			}
			else
			{
				if(neighNodeVar != INVARIANT_NODE)
					efunc->add_term1(neighNodeVar, e10, e11);
				//else
				//	efunc->add_constant(e11);				
			}
		}
	}

	double currEnergy = computeCurrEnergy();	
	double newEnergy  = efunc->minimize();
	PRINT_DOUBLE(currEnergy);
	PRINT_DOUBLE(newEnergy);
	if(newEnergy < currEnergy)
	{
		nodeAddr = 0;
		for(int y = 0; y < this->maskShape.height; y++)
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++)
		{
			Energy::Var nodeVar = this->graphNodes[nodeAddr];
			if((nodeVar != INVARIANT_NODE) &&
				(efunc->get_var(nodeVar) == 1))
			{
				ENSURE(this->maskShape.InBounds(x + newLabel.first, y + newLabel.second)); 
				this->labelImage[nodeAddr] = newLabel;
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

double GCFlow::computeCurrEnergy()
{
	double currEnergy = 0.0;

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	for(int x = 0; x < this->maskShape.width;  x++, nodeAddr++, pixelAddr += this->imgShape.nBands)
	{
		IntPair nodeLabel = this->labelImage[nodeAddr];
		currEnergy += dataCost(x, y, nodeLabel);

		for(int iEdge = 0; iEdge < GCFLOW_Connectivity; iEdge++)
		{
			int neighX = x + GCFLOW_DX[iEdge];
			int neighY = y + GCFLOW_DY[iEdge];
			if(this->maskShape.InBounds(neighX, neighY) == false) continue;

			uint neighNodeAddr     = this->maskShape.NodeIndex(neighX, neighY, 0);
			IntPair neighNodeLabel = this->labelImage[neighNodeAddr];

			currEnergy += edgeCost(nodeLabel, neighNodeLabel);
		}
	}

	return currEnergy;
}


float GCFlow::dataCost(int x, int y, IntPair label)
{
	int xx = x + label.first;
	int yy = y + label.second;
	if(this->maskShape.InBounds(xx, yy) == false)
	{
		return GCFLOW_DATA_COST_INF;
	}
	else
	{
		uint currPixelAddr  = this->imgShape.PixelIndex(x, y, 0);
		uint nextPixelAddr  = this->imgShape.PixelIndex(xx, yy, 0); 

		float costBC = 0.0f;

		if(this->imgShape.nBands == 1)
			costBC = abs(this->currImg[currPixelAddr] - this->nextImg[nextPixelAddr]);
		else
		{			
			for(int channel = 0; channel < this->imgShape.nBands; channel++)
			{
				costBC += abs(this->currImg[currPixelAddr] - this->nextImg[nextPixelAddr]);
			}
			costBC /= this->imgShape.nBands;
		}

		return (costBC * this->params.alphaBC);
	}
}

float GCFlow::edgeCost(IntPair label1, IntPair label2)
{
	float edgeCost = (abs(label1.first  - label2.first) + 
		              abs(label2.second - label2.second)) / 2.0f;
	return (edgeCost * this->params.beta);
}