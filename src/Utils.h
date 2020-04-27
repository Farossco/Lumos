#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "Request.h"
#include "Types.h"

extern const char nameUnknown[];
extern const char * lightModeName[];
extern const char * soundModeName[];
extern const char * soundCommandName[];

class Utils
{
public:
	const char * getLightModeName (LightMode mode);
	const char * getSoundModeName (uint8_t mode);
	const char * getSoundCommandName (uint8_t command);
	const char * getMessageTypeName (RequestType requestType);
	const char * getMessageTypeDisplayName (RequestType requestType);
	const char * getErrorName (RequestError requestError);
	char * getClock ();
	uint32_t map (float input, float inMin, float inMax, float outMin, float outMax);
	String ltos (uint32_t value, int base = DEC);
	RequestType getMessageTypeFromName (String message);

private:
	char clock[30];
};

extern Utils utils;

#endif // ifndef UTILS_H