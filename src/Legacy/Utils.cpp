#if 0

#include "Utils.h"
#include <TimeLib.h>

char * Utils::getClock()
{
	sprintf(clock, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d", day(), month(), year(), hour(), minute(), second());

	return clock;
}

/* Custom map function (more accurate) - Thanks to Alok Singhal from stackoverflow (https://stackoverflow.com/questions/5731863/mapping-a-numeric-range-onto-another/5732390#5732390) */
uint32_t Utils::map(float input, float inMin, float inMax, float outMin, float outMax)
{
	if ((inMin == 0 && inMax == 0) || (outMin == 0 && outMax == 0))
		return input;
	else if (inMin == inMax || outMin == outMax)
		return 0;
	else {
		double slope = 1.0 * (outMax - outMin) / (inMax - inMin);

		return outMin + round(slope * (input - inMin));
	}
}

/* Inspired from Print::printNumber */
String Utils::ltos(uint32_t value, int base)
{
	char buf[8 * sizeof(uint32_t) + 1]; /* Assumes 8-bit chars plus zero byte. */
	char *str = &buf[sizeof(buf) - 1];

	*str = '\0';

	if (base < 2)
		base = 10;

	do {
		unsigned long m = value;
		value /= base;
		char c = m - base * value;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	} while (value);

	return String(str);
}

Utils utils = Utils();

#endif /* if 0 */
