#include "Timer.h"

Timer::Timer(uint32_t period, bool oneShot)
{
	this->period = period;
	this->oneShot = oneShot;
	active = true;
	prevTime = millis();
}

bool Timer::expired()
{
	if (!active)
		return false;

	uint32_t newTime = millis();
	if ((newTime - prevTime) > period)
	{
		prevTime = newTime;
		if (oneShot)
			active = false;
		return true;
	}
	return false;
}
