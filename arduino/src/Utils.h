#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

#define COMPLEMENT_MIN 0
#define COMPLEMENT_MAX 1

// ************************************* //
// **************** IDs **************** //
// ************************************* //

// Caps
const int CAPS_NONE  = 0; // All letters in lower case
const int CAPS_FIRST = 1; // First letter in upper case
const int CAPS_ALL   = 2; // All letters in upper case

const uint8_t COMPLEMENT_TYPE_NONE = 0; // No complement
const uint8_t COMPLEMENT_TYPE_LMO  = 1; // Light mod
const uint8_t COMPLEMENT_TYPE_SCP  = 2; // Sound command parameter

// ************************************************************* //
// **************** Default, min and max values **************** //
// ************************************************************* //

// Min and Max
const int SEEKBAR_MIN = 0;   // Minimum app seek bars value
const int SEEKBAR_MAX = 100; // Maximum app seek bars value

class Utils
{
public:
	const char * lightModName (uint8_t mod, uint8_t caps);
	const char * soundModName (uint8_t mod, uint8_t caps);
	const char * soundCommandName (uint8_t mod, uint8_t caps);
	const char * infoTypeName (uint8_t infoType, boolean shortened);
	const char * errorTypeName (uint8_t infoType, boolean shortened);
	const uint8_t infoTypeComplementBounds (uint8_t infoType, uint8_t minMax);
	const uint8_t infoTypeComplementType (uint8_t infoType);
	char * clock (char * buf); // a 20-char buf is necessary
	void softwareReset ();     // Just in case
	char * reduceCharArray (char ** array, int length);
	long map (float input, float inMin, float inMax, float outMin, float outMax);
};

extern Utils utils;

#endif // ifndef UTILS_H
