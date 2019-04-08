#include "MVFlow.h"

MVFlow::MVFlowParams MVFlow::MVFlowParams::Default()
{
	MVFlowParams params;

	//bug
	params.alphaBC = 1.0f;
	params.alphaGC = 16.5f;
	params.beta    = 800.0f;

	return params;
}

void MVFlow::MVFlowParams::Dump(string prefix)
{
	const char* prefixPtr = prefix.c_str();

	printf("\n");
	printf("%sMVFlow params:\n", prefixPtr);
	printf("%s  alphaBC: %f\n", prefixPtr, alphaBC);
	printf("%s  alphaGC: %f\n", prefixPtr, alphaGC);
	printf("%s  beta: %f\n", prefixPtr, beta);
	printf("\n");
}
