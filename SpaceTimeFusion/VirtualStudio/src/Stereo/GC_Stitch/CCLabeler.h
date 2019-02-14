#ifndef __CC_LABELER_H__
#define __CC_LABELER_H__

#pragma warning(disable: 4512)

#include "Definitions.h"
#include "RegistrationDefs.h"
#include "Debug.h"
#include "Image.h"
#include <map>

typedef uint COMPONENT;
typedef	std::pair<COMPONENT, COMPONENT>       Equivalence;
typedef	std::multimap<COMPONENT, COMPONENT>   Equivalences;

class CCLabeler
{
public:
	typedef enum 
	{
		PRUNE_ALL_BUT_LARGEST
	} PruneType;
	
private:
	COMPONENT              lastComponentUsed;
	std::vector<COMPONENT> components;
	std::vector<int>       componentSizes;
	Equivalences           equivalences;
	CImageOf<COMPONENT>    componentMap;

	CShape                 maskShape;
	CImageOf<short>        &labelsImg;
	RegisteredPieces       &pieces;

	COMPONENT getNewComponent()
	{
		this->lastComponentUsed++;
		ENSURE(this->lastComponentUsed < UINT_MAX);
		this->components.push_back(this->lastComponentUsed);
		this->componentSizes.push_back(1);
		return this->lastComponentUsed;
	}

	COMPONENT findParent(COMPONENT component)
	{
		while(component != this->components[component])
		{
			component = this->components[component];
		}

		return component;
	}

public:
	CCLabeler(CImageOf<short> &labels, RegisteredPieces &regPieces);

	int PruneComponents(PruneType pruneType = PRUNE_ALL_BUT_LARGEST);

private:
	void computeEquivalences();

	COMPONENT getPixelComponent(int nodeX, int nodeY);

	void propagateEquivalences();

	int pruneAllButLargestComponent();

	COMPONENT findLargestComponent();
};

#endif

