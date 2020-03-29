#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "Request.h"

#define COMPLEMENT_MIN 0
#define COMPLEMENT_MAX 1

// ************************************* //
// **************** IDs **************** //
// ************************************* //

// Caps
const uint8_t CAPS_NONE  = 0; // All letters in lower case
const uint8_t CAPS_FIRST = 1; // First letter in upper case
const uint8_t CAPS_ALL   = 2; // All letters in upper case

const uint8_t COMPLEMENT_TYPE_NONE = 0; // No complement
const uint8_t COMPLEMENT_TYPE_LMO  = 1; // Light mod
const uint8_t COMPLEMENT_TYPE_SCP  = 2; // Sound command parameter

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

class Utils
{
public:
	String getLightModName (uint8_t mod, uint8_t caps);
	String getSoundModeName (uint8_t mod, uint8_t caps);
	String getSoundCommandName (uint8_t mod, uint8_t caps);
	const char * getMessageTypeName (RequestMessageType RequestMessageType);
	const char * getMessageTypeDisplayName (RequestMessageType RequestMessageType);
	const char * getErrorName (RequestErrorType RequestErrorType);
	const uint8_t messageTypeComplementBounds (RequestMessageType RequestMessageType, uint8_t minMax);
	const uint8_t messageTypeComplementType (RequestMessageType RequestMessageType);
	String clock ();
	uint32_t map (float input, float inMin, float inMax, float outMin, float outMax);
	String ltos (uint32_t value, int base = DEC);
	RequestMessageType getMessageTypeFromName (String message);
};

extern Utils utils;

#endif // ifndef UTILS_H