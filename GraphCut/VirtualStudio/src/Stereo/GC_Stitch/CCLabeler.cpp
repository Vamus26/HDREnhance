#include "CCLabeler.h"


CCLabeler::CCLabeler(CImageOf<short> &labels, RegisteredPieces &regPieces) : labelsImg(labels), pieces(regPieces)
{
	this->maskShape = this->labelsImg.Shape();
	ENSURE(this->maskShape.nBands == 1);
	ENSURE(this->maskShape.depth == 1);

	this->componentMap.ReAllocate(this->maskShape);
	this->componentMap.ClearPixels();
	
	uint pixelCount = this->maskShape.width * this->maskShape.height;
    this->components.reserve(pixelCount);
	this->componentSizes.reserve(pixelCount);

	this->lastComponentUsed = 0;
	this->components.push_back(0);
	this->componentSizes.push_back(0);
}

COMPONENT CCLabeler::getPixelComponent(int nodeX, int nodeY)
{
	COMPONENT pixelComponent = 0;

	const int neighourCount  = 2;
	int dx[neighourCount]    = { 0, -1};
	int dy[neighourCount]    = {-1,  0};

	for(int iNeighbour = 0; iNeighbour < neighourCount; iNeighbour++)
	{
		int neighbourX = nodeX + dx[iNeighbour];
		int	neighbourY = nodeY + dy[iNeighbour];

		if(this->maskShape.InBounds(neighbourX, neighbourY) == false) continue;
		
		uint neighbourAddr           = this->labelsImg.NodeIndex(neighbourX, neighbourY);
		COMPONENT neighbourComponent = this->componentMap[neighbourAddr];
		if(neighbourComponent == 0) continue;

		if(pixelComponent == 0)
		{
			pixelComponent = neighbourComponent;
			this->componentSizes[pixelComponent]++; 
		}
		else if(neighbourComponent != pixelComponent)
		{
			bool uniqueFind = true;
			for(Equivalences::const_iterator findResult = this->equivalences.find(pixelComponent);
				findResult != this->equivalences.end();
				findResult++)
			{
				if(findResult->first != pixelComponent) break;
				if(findResult->second == neighbourComponent)
				{
					uniqueFind = false;
					break;
				}
			}
			if(uniqueFind == true)
			{
				for(Equivalences::const_iterator findResult = this->equivalences.find(neighbourComponent);
					findResult != this->equivalences.end();
					findResult++)
				{
					if(findResult->first != neighbourComponent) break;
					if(findResult->second == pixelComponent)
					{
						uniqueFind = false;
						break;
					}
				}
			}
			if(uniqueFind == true)
			{
				this->equivalences.insert(Equivalence(pixelComponent, neighbourComponent));
			}
		}
	}

	if(pixelComponent == 0)
	{
		pixelComponent = getNewComponent();
	}

	return pixelComponent;
}

void CCLabeler::computeEquivalences()
{
	uint   nodeAddr  = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++)
		{
			if(this->labelsImg[nodeAddr] == -1)
			{
				this->componentMap[nodeAddr] = 0;
			}
			else
			{			
				this->componentMap[nodeAddr] = getPixelComponent(x, y);
			}
		}
	}
}


void CCLabeler::propagateEquivalences()
{
	for(Equivalences::iterator currEquiv =  this->equivalences.begin(); 
		currEquiv != this->equivalences.end();
		currEquiv++)
	{
		ENSURE(currEquiv->first != currEquiv->second);
		COMPONENT firstParent  = findParent(currEquiv->first);
		COMPONENT secondParent = findParent(currEquiv->second);
		if(firstParent < secondParent)
		{
			this->components[secondParent] = firstParent;
			this->componentSizes[firstParent] += this->componentSizes[secondParent];
		}
		else if(firstParent > secondParent)
		{			
			this->components[firstParent] = secondParent;
			this->componentSizes[secondParent] += this->componentSizes[firstParent];
		}
	}
}

COMPONENT CCLabeler::findLargestComponent()
{
	COMPONENT largestComponent = 0;
	int largestComponentSize   = 0; 
	uint nodeAddr  = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++)
		{
			COMPONENT pixelComponent = this->componentMap[nodeAddr];
			if(pixelComponent == 0) continue;
	
			COMPONENT parentComponent = findParent(pixelComponent);
			if(this->componentSizes[parentComponent] > largestComponentSize)
			{
				largestComponentSize = this->componentSizes[parentComponent];
				largestComponent = parentComponent;
			}
		}
	}
	
	//ENSURE(largestComponent != 0);
	return largestComponent;
}

int CCLabeler::pruneAllButLargestComponent()
{	
	int pixelsPruned = 0;
	COMPONENT largestComponent = findLargestComponent();

	if(largestComponent == 0)
	{
		REPORT("Warning: Largest component size = 0");
		return 0;
	}

	uint nodeAddr  = 0;
	for(int y = 0; y < this->maskShape.height; y++)
	{
		for(int x = 0; x < this->maskShape.width; x++, nodeAddr++)
		{
			COMPONENT pixelComponent = this->componentMap[nodeAddr];
			if(pixelComponent != 0)
			{
				ENSURE(this->labelsImg[nodeAddr] != -1);
				COMPONENT parentComponent = findParent(pixelComponent);
				if(parentComponent != largestComponent)
				{
					RegisteredPiece piece = this->pieces[this->labelsImg[nodeAddr]];
					ENSURE(piece.mask[nodeAddr] == MASK_VALID);
					this->labelsImg[nodeAddr] = -1;
					piece.mask[nodeAddr] = MASK_INVALID;					
					pixelsPruned++;
				}
			}
			else
			{
				ENSURE(this->labelsImg[nodeAddr] == -1);
			}
		}
	}

	return pixelsPruned;
}

int CCLabeler::PruneComponents(PruneType pruneType)
{
	int pixelsPruned = 0;
	computeEquivalences();
	propagateEquivalences();

	for(Equivalences::iterator currEquiv =  this->equivalences.begin(); 
		currEquiv != this->equivalences.end();
		currEquiv++)
	{
		COMPONENT firstParent  = findParent(currEquiv->first);
		COMPONENT secondParent = findParent(currEquiv->second);
		ENSURE(firstParent == secondParent);
	}
	ENSURE(this->components.size() == this->componentSizes.size());
	
	switch(pruneType)
	{
	case PRUNE_ALL_BUT_LARGEST:
		pixelsPruned = pruneAllButLargestComponent();
		break;
	default:
		REPORT_FAILURE("Unknown prune type\n");
		break;
	}

	return pixelsPruned;
}
