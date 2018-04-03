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
			return caps == 0 ? "default" : caps == 1 ? "Default" : "DEFAULT";

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

		case LIGHT_MOD_START_ANIM:
			return caps == 0 ? "start animation" : caps == 1 ? "Start animation" : "START ANIMATION";

		case LIGHT_MOD_MUSIC:
			return caps == 0 ? "music" : caps == 1 ? "Music" : "MUSIC";

		default:
			return caps == 0 ? "unknown" : caps == 1 ? "Unknown" : "UNKNOWN";
	}
}

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
		case SOUND_COMMAND_PLAY:
			return caps == 0 ? "play" : caps == 1 ? "Play" : "PLAY";

		case SOUND_COMMAND_VOLUME:
			return caps == 0 ? "volume" : caps == 1 ? "Volume" : "VOLUME";

		default:
			return caps == 0 ? "unknown" : caps == 1 ? "Unknown" : "UNKNOWN";
	}
}

const char * Utils::infoTypeName (uint8_t infoType, boolean shortened)
{
	switch (infoType)
	{
		case TYPE_TIM:
			return shortened ? "TIM" : "Time";

		case TYPE_RGB:
			return "RGB";

		case TYPE_ONF:
			return shortened ? "ONF" : "Light On/Off";

		case TYPE_POW:
			return shortened ? "POW" : "Light power";

		case TYPE_LMO:
			return shortened ? "LMO" : "Light mod";

		case TYPE_SPE:
			return shortened ? "SPE" : "Light mod speed";

		case TYPE_SMO:
			return shortened ? "SMO" : "Sound mod";

		case TYPE_VOL:
			return shortened ? "VOL" : "Sound volume";

		case TYPE_SON:
			return shortened ? "SON" : "Sound On/Off";

		case TYPE_SCO:
			return shortened ? "SCO" : "Sound command";

		default:
			return shortened ? "UNK" : "Unknown";
	}
} // Utils::infoTypeName

const char * Utils::errorTypeName (uint8_t infoType, boolean shortened)
{
	switch (infoType)
	{
		case ERR_NOE:
			return "No error";

		case ERR_OOB:
			return shortened ? "Out of bounds" : "Error: Value is out of bounds";

		case ERR_UKC:
			return shortened ? "Unknowm command" : "Error: Unknowm command";

		case ERR_UKR:
			return shortened ? "Unknown request type" : "Error: Unknown request type";

		default:
			return "Unknown error";
	}
}

const uint8_t Utils::infoTypeComplementBounds (uint8_t infoType, uint8_t minMax)
{
	switch (infoType)
	{
		case TYPE_RGB:
		case TYPE_POW:
		case TYPE_SPE:
			return minMax == COMPLEMENT_MIN ? LIGHT_MOD_MIN : LIGHT_MOD_MAX;

		case TYPE_SCO:
			return minMax == COMPLEMENT_MIN ? SOUND_COMMAND_MIN : SOUND_COMMAND_MAX;

		default:
			return minMax == COMPLEMENT_MIN ? 0 : 0;
	}
}

const uint8_t Utils::infoTypeComplementType (uint8_t infoType)
{
	switch (infoType)
	{
		case TYPE_RGB:
		case TYPE_POW:
		case TYPE_SPE:
			return COMPLEMENT_TYPE_LMO;

		case TYPE_SCO:
			return COMPLEMENT_TYPE_SCP;

		default:
			return COMPLEMENT_TYPE_NONE;
	}
}

// Requires a 23-char buffer
char * Utils::clock (char * buf)
{
	sprintf (buf, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d::%.3ld", day(), month(), year(), hour(), minute(), second(), (millis() % 1000));

	return buf;
}

// Custom map function (more accurate) - Thanks to Alok Singhal from stackoverflow (https://stackoverflow.com/questions/5731863/mapping-a-numeric-range-onto-another/5732390#5732390)
long Utils::map (float input, float inMin, float inMax, float outMin, float outMax)
{
	if (inMin == inMax || outMin == outMax)
		return 0;

	double slope = 1.0 * (outMax - outMin) / (inMax - inMin);

	return outMin + round (slope * (input - inMin));
}

char * Utils::reduceCharArray (char ** array, int length)
{
	*array += length;

	return *array;
}

Utils utils = Utils();
