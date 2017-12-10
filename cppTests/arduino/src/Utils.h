#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

// ************************************* //
// **************** IDs **************** //
// ************************************* //

// Modes
const int MODE_MIN     = 0; // -Minimum mode value-
const int MODE_DEFAULT = 0; // Default mode
const int MODE_FLASH   = 1; // Flash mode
const int MODE_STROBE  = 2; // Strobe mode
const int MODE_FADE    = 3; // Fade mode
const int MODE_SMOOTH  = 4; // Smooth mode
const int MODE_WAKEUP  = 5; // Wake up mode
const int MODE_MAX     = 5; // -Maximum mode value-
const int N_MODE       = 6; // --Number of different modes--

// Serial reception types
const int TYPE_RTM = -2; // Request : Time
const int TYPE_RIF = -1; // Request : Info
const int TYPE_UNK = 0;  // Unknown type
const int TYPE_TIM = 1;  // Provide : Time
const int TYPE_RGB = 2;  // Provide : RGB
const int TYPE_ONF = 3;  // Provide : On
const int TYPE_POW = 4;  // Provide : Power
const int TYPE_MOD = 5;  // Provide : Mode
const int TYPE_PRT = 6;  // Provide : Prayer time
const int TYPE_SPE = 7;  // Provide : Speed

// Serial reception errors
const int ERR_NOE = 0; // No error
const int ERR_OOB = 1; // Out of bound
const int ERR_UKM = 2; // Unknown mode
const int ERR_UKR = 3; // Unknown request
const int ERR_UKP = 4; // Unknown prayer

// Caps
const int CAPS_NONE  = 0; // All letters in lower case
const int CAPS_FIRST = 1; // First letter in upper case
const int CAPS_ALL   = 2; // All letters in upper case

// *************************************** //
// **************** Other **************** //
// *************************************** //

// Wake up
const int WAKEUP_HOURS   = 06;
const int WAKEUP_MINUTES = 30;
const int WAKEUP_SECONDS = 00;

// Prayer
const int PRAYER_FADE_SPEED          = 97; // Fade speed for prayer time
const int PRAYER_ALERT_DURATION      = 10; // Prayer alert duration (in minutes)
const int N_PRAYER                   = 6;  // Number of different prayer (including sunrise)
const char PRAYERS_NAME[N_PRAYER][8] = { "Fajr", "Sunrise", "Dhuhr", "Asr", "Maghrib", "Isha" };

// ************************************************************* //
// **************** Default, min and max values **************** //
// ************************************************************* //

// Default
const unsigned long DEFAULT_RGB[N_MODE] =
{ 0xFFFFFF, 0xFF0000, 0xFFFFFF, 0xFFFFFF, 0xFF0000, 0x0000FF };    // Default color on program startup
const float DEFAULT_POWER[N_MODE] = { 50, 100, 100, 100, 100, 0 }; // Default power on program startup
const int DEFAULT_SPEED[N_MODE] = { -1, 0, 0, 750, 800, 2000 };    // Default speed on program startup
const int DEFAULT_VOLUME = 30;                                     // DFPlayer default volume (0 - 30)

// Min and Max
const int MIN_SPEED[N_MODE] = { -1, 1, 1, 50, 10, 1 };         // Minimum speed or power value for each mode
const int MAX_SPEED[N_MODE] = { -1, 25, 25, 600, 1000, 1000 }; // Maximum speed or power value for each mode
const int MIN_POWER   = 0;                                     // Minimum power value
const int MAX_POWER   = 255;                                   // Maximum power value
const int SEEKBAR_MIN = 0;                                     // Minimum app seek bars value
const int SEEKBAR_MAX = 100;                                   // Maximum app seek bars value
const int MIN_VOLUME  = 0;                                     // Minimum DFPlayer volume
const int MAX_VOLUME  = 30;                                    // Maximum DFPlayer volume

class Utils
{
public:
	const char * modeName (int mode, int caps);
	const char * infoTypeName (int infoType, boolean shortened);
	const char * errorTypeName (int infoType, boolean shortened);
	char * clock (char * buf); // a 20-char buf is necessary
	void softwareReset ();     // Just in case
	int convertBoundaries (float input, float inMin, float inMax, float outMin, float outMax);
	char * reduceCharArray (char ** array, int length);
};

extern Utils utils;

#endif // ifndef UTILS_H
