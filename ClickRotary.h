#ifndef CLICK_ROTARY_H
#define CLICK_ROTARY_H

#include <Arduino.h>

class ClickRotary
{
public:
	ClickRotary(uint8_t a, uint8_t b, uint16_t maxValue = 0xffff, bool rollout = true);
	void      checkPins();
	int16_t   getValue();
	int16_t   getDelta();
private:
	uint8_t   aPin;
	uint8_t   bPin;
	int16_t   maxValue;
	bool      rollout;

	uint8_t   currBits;
	uint8_t   prevBits;
	int8_t    currDir;
	int8_t    prevDir;
	int16_t   currValue;
	int16_t   prevValue;
};

#endif
