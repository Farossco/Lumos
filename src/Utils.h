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

class Utils
{
public:
	const char * lightModName (uint8_t mod, uint8_t caps);
	const char * soundModName (uint8_t mod, uint8_t caps);
	const char * soundCommandName (uint8_t mod, uint8_t caps);
	const char * messageTypeName (MessageType MessageType);
	const char * messageTypeDisplayName (MessageType MessageType);
	const char * errorTypeName (ErrorType ErrorType);
	const uint8_t messageTypeComplementBounds (MessageType MessageType, uint8_t minMax);
	const uint8_t messageTypeComplementType (MessageType MessageType);
	String clock ();
	uint32_t map (float input, float inMin, float inMax, float outMin, float outMax);
	String ltos (uint32_t value, int base = DEC);
	MessageType getMessageTypeFromName (String message);
};

extern Utils utils;

#endif // ifndef UTILS_H