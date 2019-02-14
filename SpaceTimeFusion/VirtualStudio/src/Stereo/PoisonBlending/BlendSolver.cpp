#include "BlendSolver.h"
#include "SparseSolver.h"
#include "Timer.h"
#include "ImageProcessing.h"
#include "ImageIO.h"

#pragma warning(disable : 4127)


BlendSolver::BlendSolver(CFloatImage blendImage,
						 CImageOf<short> &labelImage, 
						 RegisteredPieces &regPieces) : pieces(regPieces)
{	
	this->labels         = labelImage.Clone();
	this->prevBlendImage = blendImage.Clone();
	this->maskShape      = this->labels.Shape();
	this->imgShape       = this->prevBlendImage.Shape();
	

	ENSURE(this->maskShape.nBands == 1);
	ENSURE(this->maskShape.depth  == 1);
	ENSURE(this->pieces.size() > 0);	
	ENSURE(this->maskShape.SameIgnoringNBands(this->imgShape));

	for(vector<RegisteredPiece>::iterator piece = this->pieces.begin();
		piece != this->pieces.end();
		piece++)
	{
		ENSURE(piece->image.Shape()         == this->imgShape);
		ENSURE(piece->filteredImage.Shape() == this->imgShape);
		ENSURE(piece->mask.Shape()          == this->maskShape);
	}

	uint nodeAddr = 0;
	for(int y = 0; y < this->imgShape.height; y++)
	{
		for(int x = 0; x < this->imgShape.width; x++, nodeAddr++)
		{
			ENSURE(this->labels[nodeAddr] < (short) this->pieces.size());
			ENSURE(this->labels[nodeAddr] >= -1);
		}
	}

	this->numOfVars = -1;
	this->gradImg.ReAllocate(this->maskShape);
	this->maskToVarMap.ReAllocate(this->maskShape);
	this->varToMaskMap.ReAllocate(this->maskShape);
}

uint BlendSolver::pickRandomConstraintNode()
{
	uint nodeCount = this->maskShape.width * this->maskShape.height;

	uint constraintIndex = 0;
	int iterations = 0;
	bool done = false;
	while(done == false)
	{
		ENSURE(iterations < 500);

		constraintIndex = myRand() % nodeCount;
		if(this->labels[constraintIndex] >= 0)
		{
			done = true;
		}
		iterations++;
	}

	return constraintIndex;
}

int BlendSolver::createMaskVarMaps(bool fillInHoles, bool ignoreHighGradLinks)
{	
	uint constrainedNodeIndex = pickRandomConstraintNode();

	int varsFound = 0;
	uint nodeAddr = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++)
		{		
			if((fillInHoles == false) && 
			   (this->labels[nodeAddr] == -1))
			{
                this->maskToVarMap[nodeAddr] = NOT_VAR;
			}
			else if((fillInHoles == true) && 
			        (this->labels[nodeAddr] != -1))
			{
                this->maskToVarMap[nodeAddr] = CONSTRAINED;
			}
			else if(nodeAddr == constrainedNodeIndex)
			{
				this->maskToVarMap[nodeAddr] = CONSTRAINED;
			}
			else if((ignoreHighGradLinks == true) &&
				    ((this->linkImgX[nodeAddr] == MASK_VALID) || (this->linkImgY[nodeAddr] == MASK_VALID)))
			{
				this->maskToVarMap[nodeAddr] = CONSTRAINED;
			}
			else
			{
				this->maskToVarMap[nodeAddr]  = varsFound;
				this->varToMaskMap[varsFound] = nodeAddr;
				varsFound++;
			}			
		}
	}

	return varsFound;
}

void BlendSolver::createBlendGradImages(int channel, bool fillInHoles, const BlendSolverParams &params)
{
	const int neighourCount     = 4;
	const int dx[neighourCount] = { 0, -1,  1,  0};
	const int dy[neighourCount] = {-1,  0,  0,  1};

	ENSURE(channel >= 0);
	ENSURE(channel < this->imgShape.nBands);

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++, pixelAddr += this->imgShape.nBands)
		{
			this->gradImg[nodeAddr] = 0;

			int nodeVar = this->maskToVarMap[nodeAddr];
			//if(nodeVar == NOT_VAR) continue;
			if(nodeVar < 0) continue;

			short label            = this->labels[nodeAddr];
			RegisteredPiece *piece = getPiece(label, fillInHoles);
            ENSURE(piece != NULL);
			ENSURE(piece->mask[nodeAddr] == MASK_VALID);

			float currChannelVal = piece->image[pixelAddr + channel];

			for(int iNeighbour = 0; iNeighbour < neighourCount; iNeighbour++)
			{
				int neighbourX = x + dx[iNeighbour];
				int neighbourY = y + dy[iNeighbour];

				if(this->maskShape.InBounds(neighbourX, neighbourY) == false) continue;
			
				uint neighbourNodeAddr  = this->labels.NodeIndex(neighbourX, neighbourY);
				int neighbourNodeVar    = this->maskToVarMap[neighbourNodeAddr];
				if(neighbourNodeVar == NOT_VAR) continue;
				float gradWeight = (neighbourNodeVar == CONSTRAINED) ? ConstraintWeight : 1.0f;

				short neighbourLabel            = this->labels[neighbourNodeAddr];
				RegisteredPiece *neighbourPiece = getPiece(neighbourLabel, fillInHoles);
	            ENSURE(neighbourPiece != NULL);

				uint neighbourPixelAddr = this->imgShape.PixelIndex(neighbourX, neighbourY, 0);
				float grad = 0;
				if(neighbourLabel == label)
				{						
					ENSURE(neighbourPiece == piece);
					grad = neighbourPiece->image[neighbourPixelAddr + channel] - currChannelVal;						
				}
				else
				{
					if(params.useStrongBlending == true)
						grad = 0.0f;
					else
					{
						float gradSum = 0;
						int gradCount = 0;
						
						if((params.ignoreHighGradLinks == true) &&
							(((dx[iNeighbour] == 0) && (this->linkImgY[neighbourNodeAddr] == MASK_VALID)) ||
							 ((dy[iNeighbour] == 0) && (this->linkImgX[neighbourNodeAddr] == MASK_VALID))))
						{
							gradSum += neighbourPiece->image[neighbourPixelAddr + channel] - currChannelVal;
							gradCount++;
						}
						else
						{
							if(piece->mask[neighbourNodeAddr] == MASK_VALID)
							{
								gradSum += piece->image[neighbourPixelAddr + channel] - currChannelVal;
								gradCount++;
							}
							if(neighbourPiece->mask[nodeAddr] == MASK_VALID)
							{
								gradSum += neighbourPiece->image[neighbourPixelAddr + channel] - 
										   neighbourPiece->image[pixelAddr + channel];
								gradCount++;
							}
							if(gradCount <= 1)
							{
								gradSum += neighbourPiece->image[neighbourPixelAddr + channel] - currChannelVal;
								gradCount++;
							}
						}
						grad = (gradSum / gradCount);
					}
				}

				this->gradImg[nodeAddr] += (grad * gradWeight);
			}
		}
	}
}

void BlendSolver::setupSystem(SparseSolver &solver, int channel)
{	
	const int EquationVars     = 5;
	const int dx[EquationVars] = { 0, -1,  0,  1,  0};
	const int dy[EquationVars] = {-1,  0,  0,  0,  1};

	int    eqVarAddr[EquationVars];
	double eqVarCoef[EquationVars];

	for(int varAddr = 0; varAddr < this->numOfVars; varAddr++)
	{
		uint nodeAddr = this->varToMaskMap[varAddr];
		ENSURE(this->maskToVarMap[nodeAddr] >= 0);
		int varX = nodeAddr % this->maskShape.width;
		int varY = nodeAddr / this->maskShape.width;

		double rhs = this->gradImg[nodeAddr];
		uint pixelAddr = this->imgShape.PixelIndex(varX, varY, 0);
		double fxy     = this->prevBlendImage[pixelAddr + channel];

		int eqVarCount = 0;
		int currNodeVarIndex = -1;
		int contrainedFound = 0;
		int symmetricVars = 0;
		for(int iEqVar = 0; iEqVar < EquationVars; iEqVar++)
		{
			if(iEqVar == 2)
			{
				ENSURE(dx[iEqVar] == 0);
				ENSURE(dy[iEqVar] == 0);
				eqVarAddr[eqVarCount] = varAddr;
				eqVarCoef[eqVarCount] = -4.0f;
				currNodeVarIndex = eqVarCount;
				eqVarCount++;
				continue;
			}			

			int neighbourX = varX + dx[iEqVar];
			int neighbourY = varY + dy[iEqVar];
			if(this->maskShape.InBounds(neighbourX, neighbourY) == false) continue;
	
			uint neighbourNodeAddr = this->labels.NodeIndex(neighbourX, neighbourY);
			int neighbourVarAddr   = this->maskToVarMap[neighbourNodeAddr];
			if(neighbourVarAddr == NOT_VAR) continue;

			if(neighbourVarAddr == CONSTRAINED)
			{
				int neighbourPixelAddr = this->imgShape.PixelIndex(neighbourX, neighbourY, 0); 
				rhs -= (this->prevBlendImage[neighbourPixelAddr + channel] * ConstraintWeight);
				contrainedFound++;
			}
			else
			{
				if((this->SymmetricSolver == false) ||
					(neighbourVarAddr >= varAddr))
				{
					eqVarAddr[eqVarCount] = neighbourVarAddr;
					eqVarCoef[eqVarCount] = 1.0f;
					eqVarCount++;
				}
				else
				{
					symmetricVars++;
				}
			}
		}

		eqVarCoef[currNodeVarIndex] = -(eqVarCount + (contrainedFound * ConstraintWeight) + symmetricVars - 1);
		for(int iEqVar = 0; iEqVar < eqVarCount - 1; iEqVar++)
		{
			ENSURE(eqVarAddr[iEqVar] < eqVarAddr[iEqVar + 1]);
		}

		ENSURE(eqVarCoef[currNodeVarIndex] <= 0.0);
		if(eqVarCoef[currNodeVarIndex] == 0.0)
		{
			eqVarCoef[currNodeVarIndex] = 1.0f;
			rhs = fxy;
		}

		solver.SetRow(varAddr, eqVarCount, fxy, rhs, eqVarAddr, eqVarCoef);
	}
}

void BlendSolver::solve(SystemType type, int channel, CFloatImage &resultImg, const BlendSolverParams params)
{
	bool fillInHoles = false;
	switch(type)
	{
	case SYSTEM_TYPE_IMG_COMPLETION:
		fillInHoles = true;
		break;

	case SYSTEM_TYPE_GRAD_BLEND:
		fillInHoles = false;
		break;

	default:
        REPORT_FAILURE("Unknown system type\n");
		break;
	}

	createBlendGradImages(channel, fillInHoles, params);

	SparseSolver solver(this->numOfVars, this->SymmetricSolver);
	setupSystem(solver, channel);
	solver.Solve(params.accuracy, params.maxIters);

	for(int varAddr = 0; varAddr < this->numOfVars; varAddr++)
	{
		uint nodeAddr  = this->varToMaskMap[varAddr];
		uint pixelAddr = nodeAddr * this->imgShape.nBands;
		resultImg[pixelAddr + channel] = (float) solver.GetSolution(varAddr);
	}
	fflush(stdout);
}

void BlendSolver::solve(SystemType sysType, CFloatImage &blendImg, const BlendSolverParams params)
{
	bool fillInHoles = false;
	switch(sysType)
	{
	case SYSTEM_TYPE_IMG_COMPLETION:
		fillInHoles = true;
		break;

	case SYSTEM_TYPE_GRAD_BLEND:
		fillInHoles = false;
		break;

	default:
        REPORT_FAILURE("Unknown system type\n");
		break;
	}

	this->numOfVars = createMaskVarMaps(fillInHoles, params.ignoreHighGradLinks);
	uint nodeAddr = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++, pixelAddr += this->imgShape.nBands)
		{
			if(this->maskToVarMap[nodeAddr] == CONSTRAINED)
			{
				for(int iChannel = 0; iChannel < this->imgShape.nBands; iChannel++)
				{
					blendImg[pixelAddr + iChannel] = this->prevBlendImage[pixelAddr + iChannel];
				}
			}
		}
	}
	
	for(int iChannel = 0; iChannel < this->imgShape.nBands; iChannel++)
	{
		solve(sysType, iChannel, blendImg, params);		
	}
}

void BlendSolver::simpleBlendInHoles(CFloatImage &blendImg, const BlendSolverParams params)
{
    CByteImage labelsMask(this->maskShape);
	uint nodeAddr = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++)
		{
			labelsMask[nodeAddr] = (this->labels[nodeAddr] == -1) ? MASK_INVALID : MASK_VALID;
		}
	}

	labelsMask = ImageProcessing::ErodeImage(labelsMask, params.holeBlendWidth, MASK_INVALID);
	CFloatImage distImg = ImageProcessing::GetDistImage(labelsMask);

	float distDenom = (params.holeBlendWidth + 1.0f) / 2.0f;
	//distDenom = distDenom * sqrt(2.0f);

	nodeAddr = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++, pixelAddr += this->imgShape.nBands)
		{
            float blendFrac = distImg[nodeAddr] / distDenom;
			blendFrac = max(min(blendFrac, 1.0f), 0.0f);

			for(int iChannel = 0; iChannel < this->imgShape.nBands; iChannel++)
			{
				blendImg[pixelAddr + iChannel] = this->prevBlendImage[pixelAddr + iChannel] * (1.0f - blendFrac) + 
					                             this->holesPiece.image[pixelAddr + iChannel] * blendFrac;
			}
		}
	}	
}

void BlendSolver::Solve(CFloatImage &blendImg, CFloatImage queryImage, BlendSolverParams &params)
{
	ENSURE(blendImg.Shape()   == this->imgShape);
	ENSURE(queryImage.Shape() == this->imgShape);
	blendImg.ClearPixels();

	if(params.ignoreHighGradLinks == true)
	{
		createLinkImages(false, params);
	}

    ENSURE(params.blendGradients == true || params.fillInHoles == true);
	if(params.blendGradients == true)
	{
		solve(SYSTEM_TYPE_GRAD_BLEND, blendImg, params);
	}
	else
	{
		blendImg = this->prevBlendImage.Clone();
	}

	if(params.fillInHoles == true)
	{
		this->prevBlendImage = blendImg.Clone();

		CByteImage blendMask(this->labels.Shape());
		int blendMaskNodeCount = blendMask.Shape().width * blendMask.Shape().height;
		for(int iNode = 0; iNode < blendMaskNodeCount; iNode++)
		{
			if(this->labels[iNode] == -1) 
				blendMask[iNode] = MASK_INVALID;
			else
				blendMask[iNode] = MASK_VALID;
		}

		this->holesPiece.id = -1;
		this->holesPiece.mask.ReAllocate(this->maskShape);
		ImageProcessing::Operate(this->holesPiece.mask, this->holesPiece.mask, MASK_VALID, ImageProcessing::OP_SET);
		this->holesPiece.image = ImageProcessing::GetFilteredImage(queryImage, this->holesPiece.mask, true,
														  		   blendImg, blendMask, true,
																   params.holesFilter);
		this->holesPiece.filteredImage = this->holesPiece.image;	

		uint nodeAddr  = 0;
		uint pixelAddr = 0;
		for(int y = 0; y < this->maskShape.height; y++)
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++, pixelAddr += this->imgShape.nBands)
		{
			if(this->labels[nodeAddr] == -1)
			{
				for(int iChannel = 0; iChannel < this->imgShape.nBands; iChannel++)
				{
					this->prevBlendImage[pixelAddr + iChannel] = this->holesPiece.filteredImage[pixelAddr + iChannel];
				}
			}
		}

		if(params.simpleHoleBlending == true)
		{
			simpleBlendInHoles(blendImg, params);
		}
		else
		{
			
			if(params.ignoreHighGradLinks == true)
			{
				createLinkImages(true, params);
			}

			solve(SYSTEM_TYPE_IMG_COMPLETION, blendImg, params);
		}
	}
}


void BlendSolver::createLinkImages(bool fillInHoles, const BlendSolverParams params)
{
	const int neighourCount     = 2;
	const int dx[neighourCount] = { 0, -1 };
	const int dy[neighourCount] = {-1,  0 };

	ENSURE(params.ignoreHighGradLinks == true);
	this->linkImgX.ReAllocate(this->maskShape);
	this->linkImgY.ReAllocate(this->maskShape);
	this->linkImgX.ClearPixels();
	this->linkImgY.ClearPixels();

	uint pixelAddr = 0;
	uint nodeAddr  = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++, pixelAddr += this->imgShape.nBands)
		{
			short nodeLabel = this->labels[nodeAddr];			
			RegisteredPiece *piece = getPiece(nodeLabel, fillInHoles);
			if(piece == NULL) continue;

			for(int iNeighbour = 0; iNeighbour < neighourCount; iNeighbour++)
			{
				int neighbourX = x + dx[iNeighbour];
				int neighbourY = y + dy[iNeighbour];

				if(this->maskShape.InBounds(neighbourX, neighbourY) == false) continue;
			
				uint neighbourNodeAddr   = this->labels.NodeIndex(neighbourX, neighbourY);
				short neighbourNodeLabel = this->labels[neighbourNodeAddr];
				if(neighbourNodeLabel == nodeLabel) continue;

				RegisteredPiece *neighbourPiece = getPiece(neighbourNodeLabel, fillInHoles);
				if(neighbourPiece == NULL) continue;
				uint neighbourPixelAddr = this->imgShape.PixelIndex(neighbourX, neighbourY, 0);
			
				ENSURE(piece->mask[nodeAddr] == MASK_VALID);
				ENSURE(neighbourPiece->mask[neighbourNodeAddr] == MASK_VALID);

				float dist = 0;
				for(int iChannel = 0; iChannel < this->imgShape.nBands; iChannel++)
				{		
					float diff = 0;
					diff = piece->image[pixelAddr + iChannel] - 
						   neighbourPiece->image[neighbourPixelAddr + iChannel];
					dist += (diff * diff);
				}
				dist = sqrt(dist);
				if(dist > params.highGradThres)
				{
					if(dx[iNeighbour] == 0)
					{
						ENSURE(dy[iNeighbour] != 0);
						this->linkImgY[nodeAddr] = MASK_VALID;
						this->linkImgY[neighbourNodeAddr] = MASK_VALID;
					}
					else
					{
						ENSURE(dy[iNeighbour] == 0);
						this->linkImgX[nodeAddr] = MASK_VALID;
						this->linkImgX[neighbourNodeAddr] = MASK_VALID;
					}
				}
			}
		}
	}

	if(params.debugDumpDir.empty() == false)
	{
		ImageIO::WriteFile(this->linkImgX, params.debugDumpDir + "linkX.tga");
		ImageIO::WriteFile(this->linkImgY, params.debugDumpDir + "linkY.tga");
	}
}
