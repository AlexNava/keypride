#include "ClickRotary.h"

ClickRotary::ClickRotary(uint8_t a, uint8_t b, uint16_t maxValue, bool rollout)
{
	aPin = a;
	bPin = b;
	this->maxValue = maxValue;
	this->rollout = rollout;

	currValue = 0;
	prevValue = 0;
	currDir = 0;
	prevDir = 0;
	pinMode(aPin, INPUT);
	pinMode(bPin, INPUT);
	currBits = digitalRead(aPin) << 1 | digitalRead(bPin);
}

void ClickRotary::checkPins()
{
	prevBits = currBits;
	currBits = digitalRead(aPin) << 1 | digitalRead(bPin);

	/* Bits values:
	 * click mid   click mid   click...
	 * 00 -> 01 -> 11 -> 10 -> 00...
	 */

	prevDir = currDir;
	switch (prevBits << 2 | currBits)
	{
	case 0b0001:
	case 0b0111:
	case 0b1110:
	case 0b1000:
		currDir = 1;
		break;
	case 0b0100:
	case 0b1101:
	case 0b1011:
	case 0b0010:
		currDir = -1;
		break;
	case 0b0011:
	case 0b1100:
		currDir = 0; // undefined
	// leave value unchanged when polling in the same position
	}

	if ((currBits == 0b00) || (currBits == 0b11)) // click
	{
		if (currDir == prevDir)
			currValue += currDir;
		currDir = 0;
	}

	if (rollout)
	{
		if (currValue > maxValue)
			currValue = 0;
		if (currValue < 0)
			currValue = maxValue;
	}
	else
	{
		if (currValue > maxValue)
			currValue = maxValue;
		if (currValue < 0)
			currValue = 0;
	}
}

int16_t ClickRotary::getValue()
{
	return currValue;
}

int16_t ClickRotary::getDelta()
{
	int16_t result = currValue - prevValue;
	if (result > (maxValue / 2))
		result -= (maxValue + 1);
	else if (result < (-maxValue / 2))
		result += (maxValue + 1);
	prevValue = currValue;
	return result;
}
