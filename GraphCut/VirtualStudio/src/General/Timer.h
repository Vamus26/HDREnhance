#ifndef TIMER_H
#define TIMER_H

#include "Debug.h"
#include <ctime>

class Timer
{
private:
	clock_t startTime;
    bool ticking;
    double secCount;

	double runningTime()
	{
		ENSURE(ticking == true);
		return ((double)(clock() - startTime)) / CLOCKS_PER_SEC;
	}

public:
	Timer(bool startNow = true);
	void Start();
	void Restart();
	double Stop();
	double GetSecondsCount();
	void DumpTime(char *debugStr = "");
	void DumpSeconds(char *debugStr = "");
};

#endif


