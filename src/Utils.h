#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "Request.h"

// ************************************************************* //
// **************** Default, min and max values **************** //
// ************************************************************* //

// Min and Max
const uint8_t SEEKBAR_MIN = 0;   // Minimum app seek bars value
const uint8_t SEEKBAR_MAX = 100; // Maximum app seek bars value

extern const char nameUnknown[];
extern const char * lightModeName[];
extern const char * soundModeName[];
extern const char * soundCommandName[];

typedef struct Bounds
{
	Bounds(int8_t min, int8_t max) : low (min), high (max){ }

	int8_t low  = 0;
	int8_t high = 0;
} Bounds;

class Utils
{
public:
	const char * getLightModeName (uint8_t mode);
	const char * getSoundModeName (uint8_t mode);
	const char * getSoundCommandName (uint8_t mode);
	const char * getMessageTypeName (RequestType requestType);
	const char * getMessageTypeDisplayName (RequestType requestType);
	const char * getErrorName (RequestError requestError);
	Bounds getComplementBounds (RequestType requestType);
	char * getClock ();
	uint32_t map (float input, float inMin, float inMax, float outMin, float outMax);
	String ltos (uint32_t value, int base = DEC);
	RequestType getMessageTypeFromName (String message);

private:
	char clock[30];
};

extern Utils utils;

#endif // ifndef UTILS_H