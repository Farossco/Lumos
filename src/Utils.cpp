#include "Utils.h"
#include <TimeLib.h>
#include "Light.h"
#include "Request.h"
#include "Sound.h"

const char nameUnknown[]        = "unknown";
const char * lightModeName[]    = { "continuous", "flash", "strobe", "fade", "smooth", "wake up", "sunset", "start animation", "music" };
const char * soundModeName[]    = { "free choice" };
const char * soundCommandName[] = { "play random", "play one", "play next", "play previous", "pause", "resume", "play dawn" };


String Utils::getLightModeName (uint8_t mode, uint8_t caps)
{
	String name;

	if (mode < (sizeof(lightModeName) / sizeof(*lightModeName)))
		name = lightModeName[mode];
	else
		name = nameUnknown;

	switch (caps)
	{
		case CAPS_FIRST:
			name.replace (name.charAt (0), name.charAt (0) - 0x20);
			break;

		case CAPS_ALL:
			name.toUpperCase();
			break;
	}

	return name;
}

String Utils::getSoundModeName (uint8_t mode, uint8_t caps)
{
	String name;

	if (mode < (sizeof(soundModeName) / sizeof(*soundModeName)))
		name = soundModeName[mode];
	else
		name = nameUnknown;

	switch (caps)
	{
		case CAPS_FIRST:
			name.replace (name.charAt (0), name.charAt (0) - 0x20);
			break;

		case CAPS_ALL:
			name.toUpperCase();
			break;
	}

	return name;
}

String Utils::getSoundCommandName (uint8_t command, uint8_t caps)
{
	String name;

	if (command < (sizeof(soundCommandName) / sizeof(*soundCommandName)))
		name = soundCommandName[command];
	else
		name = nameUnknown;

	switch (caps)
	{
		case CAPS_FIRST:
			name.replace (name.charAt (0), name.charAt (0) - 0x20);
			break;

		case CAPS_ALL:
			name.toUpperCase();
			break;
	}

	return name;
}

const char * Utils::getMessageTypeName (RequestMessageType RequestMessageType)
{
	switch (RequestMessageType)
	{
		case requestTime:
			return "TIME";

		case requestInfos:
			return "INFO";

		case provideTime:
			return "TIM";

		case soundCommand:
			return "SCO";

		case RGB:
			return "RGB";

		case LON:
			return "LON";

		case POW:
			return "POW";

		case LMO:
			return "LMO";

		case SPEED:
			return "SPE";

		case SMO:
			return "SMO";

		case VOL:
			return "VOL";

		case SON:
			return "SON";

		case DTM:
			return "DTM";

		default:
			return "UNK";
	}
} // Utils::getMessageTypeName

const char * Utils::getMessageTypeDisplayName (RequestMessageType RequestMessageType)
{
	switch (RequestMessageType)
	{
		case requestTime:
			return "Time request";

		case requestInfos:
			return "Info request";

		case provideTime:
			return "Time";

		case soundCommand:
			return "Sound command";

		case RGB:
			return "RGB";

		case LON:
			return "Light On/Off";

		case POW:
			return "Light power";

		case LMO:
			return "Light mode";

		case SPEED:
			return "Light mode speed";

		case SMO:
			return "Sound mode";

		case VOL:
			return "Sound volume";

		case SON:
			return "Sound On/Off";

		case DTM:
			return "Dawn time";

		default:
			return "Unknown";
	}
} // Utils::getMessageTypeName

const char * Utils::getErrorName (RequestErrorType RequestErrorType)
{
	switch (RequestErrorType)
	{
		case noError:
			return "No error";

		case outOfBound:
			return "Out of bounds";

		case unknownComplement:
			return "Unknowm complement";

		case unknowmType:
			return "Unknown request type";

		default:
			return "Unknown error";
	}
}

const uint8_t Utils::messageTypeComplementBounds (RequestMessageType infoType, uint8_t minMax)
{
	switch (infoType)
	{
		case RGB:
		case POW:
		case SPEED:
			return minMax == COMPLEMENT_MIN ? LIGHT_MODE_MIN : LIGHT_MODE_MAX;

		case soundCommand:
			return minMax == COMPLEMENT_MIN ? SOUND_COMMAND_MIN : SOUND_COMMAND_MAX;

		default:
			return minMax == COMPLEMENT_MIN ? 0 : 0;
	}
}

const uint8_t Utils::messageTypeComplementType (RequestMessageType infoType)
{
	switch (infoType)
	{
		case RGB:
		case POW:
		case SPEED:
			return COMPLEMENT_TYPE_LMO;

		case soundCommand:
			return COMPLEMENT_TYPE_SCP;

		default:
			return COMPLEMENT_TYPE_NONE;
	}
}

String Utils::clock ()
{
	char buf[25];

	sprintf (buf, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d::%.3ld", day(), month(), year(), hour(), minute(), second(), (millis() % 1000));

	return buf;
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

	return (String) str;
}

RequestMessageType Utils::getMessageTypeFromName (String message)
{
	message.toUpperCase();

	// Test correspondance for every type
	for (RequestMessageType i = MIN; i <= MAX; i++)
		if (message == utils.getMessageTypeName (i)) // If there is a match, we return it
			return i;

	return unknown;
}

Utils utils = Utils();