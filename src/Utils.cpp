#include "Utils.h"
#include <TimeLib.h>
#include "Light.h"
#include "Request.h"
#include "Sound.h"

const char * Utils::lightModName (uint8_t mod, uint8_t caps)
{
	switch (mod)
	{
		case LIGHT_MOD_CONTINUOUS:
			return caps == 0 ? "continuous" : caps == 1 ? "Continuous" : "CONTINUOUS";

		case LIGHT_MOD_FLASH:
			return caps == 0 ? "flash" : caps == 1 ? "Flash" : "FLASH";

		case LIGHT_MOD_STROBE:
			return caps == 0 ? "strobe" : caps == 1 ? "Strobe" : "STROBE";

		case LIGHT_MOD_FADE:
			return caps == 0 ? "fade" : caps == 1 ? "Fade" : "FADE";

		case LIGHT_MOD_SMOOTH:
			return caps == 0 ? "smooth" : caps == 1 ? "Smooth" : "SMOOTH";

		case LIGHT_MOD_DAWN:
			return caps == 0 ? "wake up" : caps == 1 ? "Wake up" : "WAKE UP";

		case LIGHT_MOD_SUNSET:
			return caps == 0 ? "sunset" : caps == 1 ? "Sunset" : "SUNSET";

		case LIGHT_MOD_START_ANIM:
			return caps == 0 ? "start animation" : caps == 1 ? "Start animation" : "START ANIMATION";

		case LIGHT_MOD_MUSIC:
			return caps == 0 ? "music" : caps == 1 ? "Music" : "MUSIC";

		default:
			return caps == 0 ? "unknown" : caps == 1 ? "Unknown" : "UNKNOWN";
	}
} // Utils::lightModName

const char * Utils::soundModName (uint8_t mod, uint8_t caps)
{
	switch (mod)
	{
		case SOUND_MOD_FREE:
			return caps == 0 ? "free choice" : caps == 1 ? "Free choice" : "FREE CHOICE";

		default:
			return caps == 0 ? "unknown" : caps == 1 ? "Unknown" : "UNKNOWN";
	}
}

const char * Utils::soundCommandName (uint8_t command, uint8_t caps)
{
	switch (command)
	{
		case SOUND_COMMAND_PLAY_RANDOM:
			return caps == 0 ? "play random" : caps == 1 ? "Play random" : "PLAY RANDOM";

		case SOUND_COMMAND_PLAY_ONE:
			return caps == 0 ? "play one" : caps == 1 ? "Play one" : "PLAY ONE";

		case SOUND_COMMAND_PLAY_NEXT:
			return caps == 0 ? "play next" : caps == 1 ? "Play next" : "PLAY NEXT";

		case SOUND_COMMAND_PLAY_PREVIOUS:
			return caps == 0 ? "play previous" : caps == 1 ? "Play previous" : "PLAY PREVIOUS";

		case SOUND_COMMAND_PAUSE:
			return caps == 0 ? "pause" : caps == 1 ? "Pause" : "PAUSE";

		case SOUND_COMMAND_RESUME:
			return caps == 0 ? "resume" : caps == 1 ? "Resume" : "RESUME";

		case SOUND_COMMAND_PLAY_DAWN:
			return caps == 0 ? "play dawn" : caps == 1 ? "Play dawn" : "PLAY DAWN";

		default:
			return caps == 0 ? "unknown" : caps == 1 ? "Unknown" : "UNKNOWN";
	}
}

const char * Utils::messageTypeName (MessageType MessageType)
{
	switch (MessageType)
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
} // Utils::messageTypeName

const char * Utils::messageTypeDisplayName (MessageType MessageType)
{
	switch (MessageType)
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
			return "Light mod";

		case SPEED:
			return "Light mod speed";

		case SMO:
			return "Sound mod";

		case VOL:
			return "Sound volume";

		case SON:
			return "Sound On/Off";

		case DTM:
			return "Dawn time";

		default:
			return "Unknown";
	}
} // Utils::messageTypeName

const char * Utils::errorTypeName (ErrorType ErrorType)
{
	switch (ErrorType)
	{
		case noError:
			return "No error";

		case outOfBound:
			return "Out of bounds";

		case unknownComplement:
			return "Unknowm complement";

		case unknowmType:
			return "Unknown request type ";

		default:
			return "Unknown error ";
	}
}

const uint8_t Utils::messageTypeComplementBounds (MessageType infoType, uint8_t minMax)
{
	switch (infoType)
	{
		case RGB:
		case POW:
		case SPEED:
			return minMax == COMPLEMENT_MIN ? LIGHT_MOD_MIN : LIGHT_MOD_MAX;

		case soundCommand:
			return minMax == COMPLEMENT_MIN ? SOUND_COMMAND_MIN : SOUND_COMMAND_MAX;

		default:
			return minMax == COMPLEMENT_MIN ? 0 : 0;
	}
}

const uint8_t Utils::messageTypeComplementType (MessageType infoType)
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

// Requires a 23-char buffer
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

MessageType Utils::getMessageTypeFromName (String message)
{
	// Test correspondance for every type
	for (MessageType i = MIN; i <= MAX; i++)
		if (message == utils.messageTypeName (i)) // If there is a match, we return it
			return i;

	return unknown;
}

Utils utils = Utils();