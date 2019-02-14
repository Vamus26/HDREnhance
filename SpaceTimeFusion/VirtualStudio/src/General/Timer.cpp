#include "Timer.h"


Timer::Timer(bool startNow)
{
	this->secCount = 0;
	this->ticking  = false;
	if(startNow == true)
	{
		Start();
	}
}

void Timer::Start()
{
	ENSURE(ticking == false);
		
	ticking = true;
	this->startTime = clock();
}

void Timer::Restart()
{
	Stop();
	this->secCount = 0;
	Start();
}


double Timer::Stop()
{
	ENSURE(this->ticking == true);

	this->secCount += runningTime();
	this->ticking = false;

	return this->secCount;
}

double Timer::GetSecondsCount()
{
	return this->secCount + ((this->ticking == true) ? runningTime() : 0);
}

void Timer::DumpTime(char *debugStr)
{
	int totalSeconds    = (int) GetSecondsCount();
	
	int seconds = (totalSeconds % 60);
	int minutes = ((totalSeconds / 60) % 60);
	int hours   = (totalSeconds / 3600);

	printf("%s %02i:%02i:%02i\n", debugStr, hours, minutes, seconds);
}

void Timer::DumpSeconds(char *debugStr)
{
	double time = GetSecondsCount();
	printf("%s %.4lf\n", debugStr, time);
}


