#include "Flow.h"
#include "Timer.h"

void Flow::processCurrCamFlow()
{
	Timer timer;
	for(int yOffset = -this->flowParams.searchWinSize; yOffset <= this->flowParams.searchWinSize; yOffset++)
	{
		for(int xOffset = -this->flowParams.searchWinSize; xOffset <= this->flowParams.searchWinSize; xOffset++)
		{
			getCurrViewDataCostImg(xOffset, yOffset);
		}
	}	
	timer.DumpSeconds("Done.");
}