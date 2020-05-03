#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

class Utils
{
public:
	char * getClock ();
	uint32_t map (float input, float inMin, float inMax, float outMin, float outMax);
	String ltos (uint32_t value, int base = DEC);

private:
	char clock[25];
};

extern Utils utils;

#endif // ifndef UTILS_H