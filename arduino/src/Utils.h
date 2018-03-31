#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

// ************************************* //
// **************** IDs **************** //
// ************************************* //

// Serial reception types
const int TYPE_RTM = -2; // Request : Time
const int TYPE_RIF = -1; // Request : Info
const int TYPE_UNK = 0;  // Unknown type
const int TYPE_TIM = 1;  // Provide : Time
const int TYPE_RGB = 2;  // Provide : RGB
const int TYPE_ONF = 3;  // Provide : On
const int TYPE_POW = 4;  // Provide : Power
const int TYPE_MOD = 5;  // Provide : Mod
const int TYPE_SPE = 6;  // Provide : Speed

// Serial reception errors
const int ERR_NOE = 0; // No error
const int ERR_OOB = 1; // Out of bound
const int ERR_UKM = 2; // Unknown mod
const int ERR_UKR = 3; // Unknown request

// Caps
const int CAPS_NONE  = 0; // All letters in lower case
const int CAPS_FIRST = 1; // First letter in upper case
const int CAPS_ALL   = 2; // All letters in upper case

// ************************************************************* //
// **************** Default, min and max values **************** //
// ************************************************************* //

// Min and Max
const int SEEKBAR_MIN      = 0;                               // Minimum app seek bars value
const int SEEKBAR_MAX      = 100;                             // Maximum app seek bars value
const int MIN_VOLUME       = 0;                               // Minimum DFPlayer volume
const int MAX_VOLUME       = 30;                              // Maximum DFPlayer volume

class Utils
{
public:
	const char * modName (int mod, int caps);
	const char * infoTypeName (int infoType, boolean shortened);
	const char * errorTypeName (int infoType, boolean shortened);
	char * clock (char * buf); // a 20-char buf is necessary
	void softwareReset ();     // Just in case
	char * reduceCharArray (char ** array, int length);
	long map (float input, float inMin, float inMax, float outMin, float outMax);
};

extern Utils utils;

#endif // ifndef UTILS_H
