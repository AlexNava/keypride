#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

class Timer
{
public:
	Timer(uint32_t period, bool oneShot = false);
	bool expired();
private:
	uint32_t period;
	uint32_t prevTime;
	bool active;
	bool oneShot;
};

#endif
