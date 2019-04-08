#include "GCFlow.h"

GCFlow::GCFlowParams GCFlow::GCFlowParams::Default()
{
	GCFlowParams params;

	params.alphaBC = 1.0f;
	params.alphaGC = 1.0f;
	params.beta    = 1.0f;

	params.startU = -1;
	params.endU   =  1;
	params.startV = -1;
	params.endV   =  1;

	params.maxIterations = 3;

	return params;
}

void GCFlow::GCFlowParams::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sGCFlow params:\n", prefixPtr);
	printf("%s  alphaBC: %f\n", prefixPtr, alphaBC);
	printf("%s  alphaGC: %f\n", prefixPtr, alphaGC);
	printf("%s  beta: %f\n", prefixPtr, beta);
	printf("%s  search range [%i, %i] - [%i, %i]: %f\n", prefixPtr, 
			startU, endU, startV, endV);
	printf("%s  maxIterations: %i\n", prefixPtr, maxIterations);
	printf("\n");
}
