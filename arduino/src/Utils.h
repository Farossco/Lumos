#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

// ************************************* //
// **************** IDs **************** //
// ************************************* //

// Mods
const int MOD_MIN     = 0; // -Minimum mod value-
const int MOD_DEFAULT = 0; // Default mod
const int MOD_FLASH   = 1; // Flash mod
const int MOD_STROBE  = 2; // Strobe mod
const int MOD_FADE    = 3; // Fade mod
const int MOD_SMOOTH  = 4; // Smooth mod
const int MOD_DAWN    = 5; // Dawn mod
const int MOD_MAX     = 5; // -Maximum mod value-
const int N_MOD       = 6; // --Number of different mods--

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

// Default
const unsigned long DEFAULT_RGB[N_MOD] =
{ 0xFFFFFF, 0xFF0000, 0xFFFFFF, 0xFFFFFF, 0xFF0000, 0x0000FF };   // Default color on program startup
const float DEFAULT_POWER[N_MOD] = { 50, 100, 100, 100, 100, 0 }; // Default power on program startup
const int DEFAULT_SPEED[N_MOD]   = { -1, 10, 10, 300, 500, 500 }; // Default speed on program startup
const int DEFAULT_VOLUME         = 30;                            // DFPlayer default volume (0 - 30)

// Min and Max
const int MIN_SPEED[N_MOD] = { -1, 1, 1, 50, 10, 1 };         // Minimum speed or power value for each mod
const int MAX_SPEED[N_MOD] = { -1, 25, 25, 600, 1000, 1000 }; // Maximum speed or power value for each mod
const int MIN_POWER        = 0;                               // Minimum power value
const int MAX_POWER        = 255;                             // Maximum power value
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
