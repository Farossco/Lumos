#include "Utils.h"
#include <TimeLib.h>
#include "Light.h"
#include "Request.h"
#include "Sound.h"

const char nameUnknown[]              = "Unknown";
const char * lightModeName[]          = { "Continuous", "Flash", "Strobe", "Fade", "Smooth", "Wake up", "Sunset", "Start animation", "Music" };
const char * soundModeName[]          = { "Free choice" };
const char * soundCommandName[]       = { "Play random", "Play one", "Play next", "Play previous", "Pause", "Resume", "Play dawn" };
const char * messageTypeName[]        = { nameUnknown, "TIME", "INFO", "SCO", "TIM", "LON", "RGB", "POW", "LMO", "SPE", "SON", "SMO", "VOL", "DTM" };
const char * messageTypeDisplayName[] = { nameUnknown, "Time request", "Info request", "Sound command", "Time", "Light On/Off", "RGB", "Light power", "Light mode", "Light mode speed", "Sound On/Off", "Sound mode", "Sound volume", "Dawn time" };
const char * errorName[]              = { "No error", "Out of bounds", "Unknowm complement", "Unknown request type", "Bad String" };

#define getArrayString(in, array) ((in < (sizeof(array) / sizeof(* array))) ? array[in] : nameUnknown)

const char * Utils::getLightModeName (LightMode mode){ return getArrayString (mode, lightModeName); }

const char * Utils::getSoundModeName (uint8_t mode){ return getArrayString (mode, soundModeName); }

const char * Utils::getSoundCommandName (uint8_t command){ return getArrayString (command, soundCommandName); }

const char * Utils::getMessageTypeName (RequestType requestType){ return getArrayString (requestType, messageTypeName); }

const char * Utils::getMessageTypeDisplayName (RequestType requestType){ return getArrayString (requestType, messageTypeDisplayName); }

const char * Utils::getErrorName (RequestError requestError){ return getArrayString (requestError, errorName); }

char * Utils::getClock ()
{
	sprintf (clock, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d::%.3ld", day(), month(), year(), hour(), minute(), second(), (millis() % 1000));

	return clock;
}

// Custom map function (more accurate) - Thanks to Alok Singhal from stackoverflow (https://stackoverflow.com/questions/5731863/mapping-a-numeric-range-onto-another/5732390#5732390)
uint32_t Utils::map (float input, float inMin, float inMax, float outMin, float outMax)
{
	if ((inMin == 0 && inMax == 0) || (outMin == 0 && outMax == 0))
		return input;
	else if (inMin == inMax || outMin == outMax)
		return 0;
	else
	{
		double slope = 1.0 * (outMax - outMin) / (inMax - inMin);

		return outMin + round (slope * (input - inMin));
	}
}

// Inspired from Print::printNumber
String Utils::ltos (uint32_t value, int base)
{
	char buf[8 * sizeof(uint32_t) + 1]; // Assumes 8-bit chars plus zero byte.
	char * str = &buf[sizeof(buf) - 1];

	*str = '\0';

	if (base < 2)
		base = 10;

	do
	{
		unsigned long m = value;
		value /= base;
		char c = m - base * value;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	}
	while(value);

	return String (str);
}

RequestType Utils::getMessageTypeFromName (String message)
{
	// Test correspondance for each type
	for (RequestType i = RequestType::MIN; i <= RequestType::MAX; i++)
		if (message == utils.getMessageTypeName (i)) // If there is a match, we return it
			return i;

	return RequestType::unknown;
}

Utils utils = Utils();