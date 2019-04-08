#include "PiecesBlender.h"
#include "ImageProcessing.h"
#include <float.h>
#include <math.h>

#include "MathHelper.h"

#pragma warning(disable:4127)

PiecesBlender::	PiecesBlender(CFloatImage &targetImage, RegisteredPieces &registeredPieces,
							  const CImageOf<short> &disparitySrcs, 
							  const CFloatImage &compositeDisparityMap, 							  
							  const vector<CFloatImage> &srcImages, 
							  const vector<Images> &interSourceDisparityMaps,
							  const bool useDisparityObjective) : objectiveImage(targetImage),
															      pieces(registeredPieces),
																  disparitySources(disparitySrcs),
																  disparityMap(compositeDisparityMap),
																  sourceImages(srcImages),
																  interSourceDisparities(interSourceDisparityMaps),
																  useDisparityObj(useDisparityObjective)
{
	this->imgShape  = this->objectiveImage.Shape();
	this->maskShape = CShape(this->imgShape.width, this->imgShape.height, 1);

	ENSURE(this->imgShape.depth == 1);
	ENSURE(this->imgShape.width  > 0);
	ENSURE(this->imgShape.height > 0);

	ENSURE(this->pieces.size() > 0);
	ENSURE(this->pieces.size() <= SHRT_MAX);

	int connectivity = Connectivity;
	ENSURE((connectivity == 4) || (connectivity == 2));

	int iPiece = 0;
	for(vector<RegisteredPiece>::iterator piece = this->pieces.begin();
		piece != this->pieces.end();
		piece++, iPiece++)
	{
		ENSURE(this->imgShape  == piece->image.Shape());
		ENSURE(this->imgShape  == piece->filteredImage.Shape());
		ENSURE(this->maskShape == piece->mask.Shape());
		ENSURE(this->maskShape == piece->colDistSqrImg.Shape());		
	}

	ENSURE((typeid(Graph::captype) == typeid(float)) ||
		   (typeid(Graph::captype) == typeid(double)));

	this->labelImage.ReAllocate(this->maskShape);
	this->graphNodes.ReAllocate(this->maskShape);
	this->spaceMask.ReAllocate(this->maskShape);

	if(this->useDisparityObj == true)
	{
		ENSURE(this->maskShape == this->disparitySources.Shape());
		ENSURE(this->imgShape.SameIgnoringNBands(this->disparityMap.Shape()));
		ENSURE(this->disparityMap.Shape().nBands == 3);		
		
		//check
		/*
		if(this->sourceImages.size() > 1)
		{
			ENSURE(this->interSourceDisparities.size() == this->sourceImages.size());
			for(uint iSearch = 0; iSearch < this->interSourceDisparities.size(); iSearch++)
			{
				const Images &currSearchDisparities = this->interSourceDisparities[iSearch];
				ENSURE(currSearchDisparities.size() == this->sourceImages.size());
				for(uint jSearch = 0; jSearch < currSearchDisparities.size(); jSearch++)
				{
					if(iSearch == jSearch) continue;

					ENSURE(currSearchDisparities[jSearch].Shape().nBands == 3);
					ENSURE(currSearchDisparities[jSearch].Shape().SameIgnoringNBands(this->sourceImages[iSearch].Shape()));
				}
			}
		}
		*/
	}

	objPiece.id = -1;
	objPiece.image = this->objectiveImage;
	objPiece.filteredImage = this->objectiveImage;;
	objPiece.mask.ReAllocate(this->maskShape);
	ImageProcessing::Operate(objPiece.mask, objPiece.mask, MASK_VALID, ImageProcessing::OP_SET);
	objPiece.ComputeEdgeImages();

	initLabels();	
}

void PiecesBlender::initLabels()
{
	this->spaceMask.ClearPixels();

	vector<short> labelChoices(this->pieces.size());
	uint nodeAddr = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++)
		{
			this->labelImage[nodeAddr] = -1;
			int choicesFound = 0;
			for(short iLabel = 0; iLabel < (short) this->pieces.size(); iLabel++)
			{
				RegisteredPiece &piece = this->pieces[iLabel];
				if(piece.mask[nodeAddr] == MASK_VALID)
				{
					//this->labelImage[nodeAddr] = iLabel;
					//break;
					labelChoices[choicesFound] = iLabel;
					choicesFound++;
				}
			}
			if(choicesFound > 0)
			{
				this->labelImage[nodeAddr] = labelChoices[myRand() % choicesFound];
				this->spaceMask[nodeAddr] = MASK_VALID;
			}
		}
	}
}

//bug - handle occ label
inline float PiecesBlender::dataCostTemporal(RegisteredPiece &piece, uint nodeAddr, const PiecesBlenderParams params)
{
	short temporalSource = this->disparitySources[nodeAddr];
	if(temporalSource == -1) return 0;

	uint disparityAddrU = nodeAddr * this->disparityMap.Shape().nBands;
	uint disparityAddrV = disparityAddrU + 1;
	uint disparityAddrE = disparityAddrU + 2;
	
	float srcX = piece.disparityMap[disparityAddrU];
	float srcY = piece.disparityMap[disparityAddrV];
	ENSURE(srcX != UNDEFINED_DISPARITY);
	ENSURE(srcY != UNDEFINED_DISPARITY);

	float disparityAccuracy = this->disparityMap[disparityAddrE];
	ENSURE(disparityAccuracy >= 0.0f);
	ENSURE(disparityAccuracy <= 1.0f);

	float srcXX = this->disparityMap[disparityAddrU];
	float srcYY = this->disparityMap[disparityAddrV];                    
	ENSURE(srcXX != UNDEFINED_DISPARITY);
	ENSURE(srcYY != UNDEFINED_DISPARITY);

	float dist = 5.0f;
	bool  distComputable  = true;
	float interSourcePen  = 0.0f;
	if(piece.id != temporalSource)
	{
		const CFloatImage &searchImage = this->sourceImages[temporalSource];
		float colorDist = 0;
		uint pixelAddr  = this->imgShape.nBands * nodeAddr;
		for(int iBand = 0; iBand < this->imgShape.nBands; iBand++)
		{
			float diff;
			diff = piece.image[pixelAddr + iBand] - 
				   ImageProcessing::BilinearIterpolation(searchImage,
														 srcXX, srcYY, iBand);
			colorDist += (diff * diff);
		}
		dist += sqrt(colorDist);

		//CFloatImage &interSearchDisparity = this->interSourceDisparities[temporalSource][piece.id];
		//int mapX = NEAREST_INT(srcXX);
		//int mapY = NEAREST_INT(srcYY);
		//ENSURE(this->sourceImages[temporalSource].Shape().InBounds(mapX, mapY));

		//float srcXXX = interSearchDisparity.Pixel(mapX, mapY, 0);
		//float srcYYY = interSearchDisparity.Pixel(mapX, mapY, 1);

		//if((srcXXX != UNDEFINED_DISPARITY) &&
		//   (srcYYY != UNDEFINED_DISPARITY))
		//{
		//	srcXX = srcXXX;
		//	srcYY = srcYYY;
		//  ENSURE(this->sourceImages[piece.id].Shape().InBounds(srcXX, srcYY));
		//  float interSourceJumpAccuracy = interSearchDisparity.Pixel(mapX, mapY, 2);
		//	ENSURE(interSourceJumpAccuracy >= 0.0f);
		//	ENSURE(interSourceJumpAccuracy <= 1.0f);
		//	interSourcePen = 1.0f - interSourceJumpAccuracy;
		//	interSourcePen += 0.5f;
		//}
		//else
		{
			distComputable = false;
		}
	}

	if(distComputable == true)
	{
		float diff;
		diff = srcX - srcXX;
		dist = diff * diff;
		diff = srcY - srcYY;
		dist += diff * diff;
		dist = sqrt(dist);
		dist += interSourcePen;
		dist = std::min<float>(dist, 5.0f);
	}
	dist *= disparityAccuracy;
	dist *= params.temporalCostScale;

	return dist;
}


inline float PiecesBlender::dataCost(uint pixelAddr, uint nodeAddr, short label, const PiecesBlenderParams params)
{
	ENSURE(this->spaceMask[nodeAddr] == MASK_VALID);
	TOUCH(pixelAddr);

	if(label == -1)
	{
		return params.occlusionCost * params.dataCostScale;
	}


	RegisteredPiece &piece = this->pieces[label];
	float dataCost = 0;
	if(piece.mask[nodeAddr] == MASK_INVALID)
	{
		dataCost = DATA_COST_INF;
	}
	else
	{
		//float dist = 0;
		//for(int iBand = 0; iBand < this->imgShape.nBands; iBand++)
		//{
		//	float diff;
		//	diff = (piece.filteredImage[pixelAddr + iBand] - this->objectiveImage[pixelAddr + iBand]);
		//	dist += (diff * diff);
		//}
		//dataCost = sqrt(dist);

		dataCost = sqrt(piece.colDistSqrImg[nodeAddr]);
		dataCost = dataCost * params.dataCostScale;

		//bug - temporal cost with occulusion?
		if(this->useDisparityObj == true)
		{
			dataCost += dataCostTemporal(piece, nodeAddr, params);
		}

		ENSURE(dataCost <= DATA_COST_INF);
	}

	return dataCost;
}

inline float PiecesBlender::edgeCost(uint pixelAddrP, uint nodeAddrP, 
									 uint pixelAddrQ, uint nodeAddrQ, 
									 short label1, short label2, int neighbourType,
									 const PiecesBlenderParams params)
{
	ENSURE(this->spaceMask[nodeAddrP] == MASK_VALID);
	ENSURE(this->spaceMask[nodeAddrQ] == MASK_VALID);

	RegisteredPiece *piece1 = (label1 == -1) ? &this->objPiece : &(this->pieces[label1]);
	RegisteredPiece *piece2 = (label2 == -1) ? &this->objPiece : &(this->pieces[label2]);
	if(piece1->mask[nodeAddrP] == MASK_INVALID) return EDGE_COST_INF;
	if(piece2->mask[nodeAddrQ] == MASK_INVALID) return EDGE_COST_INF;
	if(label1 == label2) return 0;

	uint piece1Pixel1 = pixelAddrP;
	uint piece1Pixel2 = pixelAddrQ;
	uint piece2Pixel1 = pixelAddrP;
	uint piece2Pixel2 = pixelAddrQ;
	uint piece1Node1  = nodeAddrP;
	uint piece1Node2  = nodeAddrQ;
	uint piece2Node1  = nodeAddrP;
	uint piece2Node2  = nodeAddrQ;
	if(piece1->mask[nodeAddrQ] == MASK_INVALID)
	{
		piece1Pixel2 = pixelAddrP;
		piece1Node2  = nodeAddrP;
	}
	if(piece2->mask[nodeAddrP] == MASK_INVALID)
	{
		piece2Pixel1 = pixelAddrQ;
		piece2Node1  = nodeAddrQ;
	}

	float dist1 = 0;
	float dist2 = 0;
	for(int iBand = 0; iBand < this->imgShape.nBands; iBand++)
	{
		//bug
		if((label1 == -1) || (label2 == -1))
		{
			float diff;
			diff = (piece1->filteredImage[piece1Pixel1 + iBand] - piece2->filteredImage[piece2Pixel1 + iBand]);
			dist1 += (diff * diff);

			diff = (piece1->filteredImage[piece1Pixel2 + iBand] - piece2->filteredImage[piece2Pixel2 + iBand]);
			dist2 += (diff * diff);
		}
		else
		{
			float diff;
			diff = (piece1->image[piece1Pixel1 + iBand] - piece2->image[piece2Pixel1 + iBand]);
			dist1 += (diff * diff);

			diff = (piece1->image[piece1Pixel2 + iBand] - piece2->image[piece2Pixel2 + iBand]);
			dist2 += (diff * diff);
		}
	}
	float edgeCost = (sqrt(dist1) + sqrt(dist2)) / 2.0f;
	edgeCost = edgeCost / neighbourDist[neighbourType];

	if((edgeCost > 0.004) &&
	   (params.useGradCosts == true))
	{
		ENSURE(neighbourType < Connectivity);	

		CFloatImage edgeImage1;
		CFloatImage edgeImage2;
		switch(neighbourType)
		{
		case HORIZONTAL_EDGE:
			edgeImage1 = piece1->sobelImageHE;
			edgeImage2 = piece2->sobelImageHE;
			break;
		case VERTICAL_EDGE:
			edgeImage1 = piece1->sobelImageVE;
			edgeImage2 = piece2->sobelImageVE;
			break;
		case POS_DIAG_EDGE:
			edgeImage1 = piece1->sobelImagePDE;
			edgeImage2 = piece2->sobelImagePDE;
			break;
		case NEG_DIAG_EDGE:
			edgeImage1 = piece1->sobelImageNDE;
			edgeImage2 = piece2->sobelImageNDE;
			break;
		default:
			REPORT_FAILURE("Unknown edge type\n");
			break;
		}

		float edgeResponse  = 0;
		edgeResponse       += (edgeImage1[piece1Node1] + edgeImage1[piece1Node2]);
		edgeResponse       += (edgeImage2[piece2Node1] + edgeImage2[piece2Node2]);
		edgeResponse       /= 4;

		float maxEdgeResponse = sqrt((float)this->imgShape.nBands);
		//edgeCost /= edgeResponse;
		//edgeCost = min(edgeCost, EDGE_COST_INF);
		edgeCost *= (maxEdgeResponse - edgeResponse);

		ENSURE(edgeCost <= EDGE_COST_INF);
	}

	return edgeCost;
}


double PiecesBlender::computeEnergy(const PiecesBlenderParams params) 
{
	double energy  = 0;
	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	for(int x = 0; x < this->maskShape.width; x++, nodeAddr++, pixelAddr += this->imgShape.nBands)
	{
		if(this->spaceMask[nodeAddr] == MASK_INVALID) continue;
		short nodeLabel = this->labelImage[nodeAddr];
		energy += dataCost(pixelAddr, nodeAddr, nodeLabel, params);

		for(int iEdge = 0; iEdge < Connectivity; iEdge++)
		{
			int neighbourX = x + neighbourDX[iEdge];
			int neighbourY = y + neighbourDY[iEdge];
			if(this->maskShape.InBounds(neighbourX, neighbourY) == false) continue;
			
			uint  neighbourNodeAddr = this->maskShape.NodeIndex(neighbourX, neighbourY);
			if(this->spaceMask[neighbourNodeAddr] == MASK_INVALID) continue;
			short neighbourLabel    = this->labelImage[neighbourNodeAddr];				
			uint neighbourPixelAddr = this->imgShape.PixelIndex(neighbourX, neighbourY, 0);

			energy += edgeCost(pixelAddr, nodeAddr,
							   neighbourPixelAddr, neighbourNodeAddr, 
							   nodeLabel, neighbourLabel,
							   iEdge, 
							   params);
		}
	}
	
	return energy;
}

void error_function(char *msg) 
{
	REPORT_FAILURE("Graphcut error: %s\n", msg);  
}

#define INVARIANT_NODE ((Energy::Var) 0)

void PiecesBlender::expand(short label, const PiecesBlenderParams params) 
{
	RegisteredPiece *labelPiece = (label == -1)? &this->objPiece : &(this->pieces[label]);

	Energy *efunc = new Energy(error_function);

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++, pixelAddr += this->imgShape.nBands)
		{
			if(this->spaceMask[nodeAddr] == MASK_INVALID)
			{
				this->graphNodes[nodeAddr] = INVARIANT_NODE;
				continue;
			}

			short nodeLabel = this->labelImage[nodeAddr];
			if((nodeLabel == label) ||
			   (labelPiece->mask[nodeAddr] == MASK_INVALID))			   
			{
				this->graphNodes[nodeAddr] = INVARIANT_NODE;
				efunc->add_constant(dataCost(pixelAddr, nodeAddr, nodeLabel, params));
			}
			else
			{
				this->graphNodes[nodeAddr] = efunc->add_variable();
				efunc->add_term1(this->graphNodes[nodeAddr],
								 dataCost(pixelAddr, nodeAddr, nodeLabel, params),
								 dataCost(pixelAddr, nodeAddr, label, params));
			}
		}
	}
	
	nodeAddr  = 0;
	pixelAddr = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++, pixelAddr += this->imgShape.nBands)
		{
			if(this->spaceMask[nodeAddr] == MASK_INVALID) continue;
			short nodeLabel = this->labelImage[nodeAddr];			
			Energy::Var nodeVar = this->graphNodes[nodeAddr];

			for(int iEdge = 0; iEdge < Connectivity; iEdge++)
			{
				int neighbourX = x + neighbourDX[iEdge];
				int neighbourY = y + neighbourDY[iEdge];
				if(this->maskShape.InBounds(neighbourX, neighbourY) == false) continue;

				uint  neighbourNodeAddr  = this->maskShape.NodeIndex(neighbourX, neighbourY, 0);
				if(this->spaceMask[neighbourNodeAddr] == MASK_INVALID) continue;

				short neighbourNodeLabel = this->labelImage[neighbourNodeAddr];				
				uint neighbourPixelAddr = this->imgShape.PixelIndex(neighbourX, neighbourY, 0);
				Energy::Var neighbourNodeVar = this->graphNodes[neighbourNodeAddr];

				float e00 = 0, e01 = 0, e10 = 0, e11 = 0;

				e00 = edgeCost(pixelAddr, nodeAddr,
							   neighbourPixelAddr, neighbourNodeAddr,
							   nodeLabel, neighbourNodeLabel,
							   iEdge, params);

				if (nodeVar != INVARIANT_NODE)
				{
					e10 = edgeCost(pixelAddr, nodeAddr,
								   neighbourPixelAddr, neighbourNodeAddr,
								   label, neighbourNodeLabel,
								   iEdge, params);
				}

				if (neighbourNodeVar != INVARIANT_NODE)
				{
					e01 = edgeCost(pixelAddr, nodeAddr,
								   neighbourPixelAddr, neighbourNodeAddr,
								   nodeLabel, label,
								   iEdge, params);
				}

				e11 = 0;
				
				if (nodeVar != INVARIANT_NODE)
				{
					if (neighbourNodeVar != INVARIANT_NODE)
					{
						efunc->add_term2(nodeVar, neighbourNodeVar, e00, e01, e10, e11);
					}
					else
					{
						efunc->add_term1(nodeVar, e00, e10);
					}
				}
				else
				{
					if (neighbourNodeVar != INVARIANT_NODE)
					{		
						efunc->add_term1(neighbourNodeVar, e00, e01);
					}
					else
					{
						efunc->add_constant(e00);
					}
				}
			}
		}
	}
	

	double newEnergy = efunc->minimize();
	if(newEnergy < this->currentEnergy)
	{
		this->currentEnergy = newEnergy;
		nodeAddr  = 0;
		for(int y = 0; y < this->maskShape.height; y++)
		{
			for(int x = 0; x < this->maskShape.width; x++, nodeAddr++)
			{
				Energy::Var nodeVar = this->graphNodes[nodeAddr];
				if((nodeVar != INVARIANT_NODE) &&
					(efunc->get_var(nodeVar) == 1))
				{
					ENSURE(labelPiece->mask[nodeAddr] == MASK_VALID);
					this->labelImage[nodeAddr] = label;
				}
			}
		}
	}

	delete efunc;
}

void PiecesBlender::ComputeFinalBlend(CFloatImage &dstImg,
									  const CImageOf<short> &labelImage,
									  const RegisteredPieces &pieces, 
									  const CByteImage &dstMask, bool fillMask)
{
	ENSURE(pieces.size() > 0);
	CShape imgShape = labelImage.Shape();
	imgShape.nBands = pieces[0].image.Shape().nBands;

	dstImg.ReAllocate(imgShape);
	dstImg.ClearPixels();

	uint nodeAddr  = 0;
	uint pixelAddr = 0;
	for(int y = 0; y < imgShape.height; y++)
	{
		for(int x = 0; x < imgShape.width; x++, nodeAddr++, pixelAddr += imgShape.nBands)
		{
			short nodeLabel = labelImage[nodeAddr];
			if(fillMask == true)
			{
				dstMask[nodeAddr] = (nodeLabel < 0) ? MASK_INVALID : MASK_VALID;
			}
			if(nodeLabel >= 0)
			{
				const RegisteredPiece &piece = pieces[nodeLabel];
				ENSURE(piece.mask[nodeAddr] == MASK_VALID);
				for(int iBand = 0; iBand < imgShape.nBands; iBand++)
				{
					dstImg[pixelAddr + iBand] = piece.image[pixelAddr + iBand];
				}
			}
		}
	}
}


void PiecesBlender::Blend(CFloatImage &dstImg, CImageOf<short> &dstLabels, PiecesBlender::PiecesBlenderParams params)
{
	ENSURE(params.maxIterations >= 1);
	ENSURE(params.energyEpsilon >= 0);

	if(Connectivity == 4)
	{
		params.dataCostScale *= 1.707f;
	}

	int maxStagnation = (int) this->pieces.size();
	int stagnation = 0;

	printf("Graphcut Optimization...\n");
	this->currentEnergy = computeEnergy(params);
	for(int iter = 0; 
		(iter < params.maxIterations) && (stagnation < maxStagnation); 
		iter++)
	{
		for(short iLabel = 0;
			(iLabel < (short) this->pieces.size()) && (stagnation < maxStagnation); 
			iLabel++)
		{
			double oldEnergy = this->currentEnergy;
			expand(iLabel, params);
			if(oldEnergy - params.energyEpsilon <= this->currentEnergy)
			{
				printf("> ");
				stagnation++;
			}
			else
			{
				printf("< ");
				stagnation = 0;				
			}
			fflush(stdout);
		}
	}
	printf("\n");
	PRINT_DOUBLE(this->currentEnergy);

	ComputeFinalBlend(dstImg, this->labelImage, this->pieces);
	dstLabels = this->labelImage;
}

