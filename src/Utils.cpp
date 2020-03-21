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

const char * Utils::messageTypeName (ReqMes messageType, boolean shortened)
{
	switch (messageType)
	{
		case TIM:
			return shortened ? "TIM" : "Time";

		case RGB:
			return "RGB";

		case LON:
			return shortened ? "LON" : "Light On/Off";

		case POW:
			return shortened ? "POW" : "Light power";

		case LMO:
			return shortened ? "LMO" : "Light mod";

		case SPEED:
			return shortened ? "SPE" : "Light mod speed";

		case SMO:
			return shortened ? "SMO" : "Sound mod";

		case VOL:
			return shortened ? "VOL" : "Sound volume";

		case SON:
			return shortened ? "SON" : "Sound On/Off";

		case DTM:
			return shortened ? "DTM" : "Dawn time";

		case SCO:
			return shortened ? "SCO" : "Sound command";

		default:
			return shortened ? "UNK" : "Unknown";
	}
} // Utils::messageTypeName

const char * Utils::errorTypeName (ReqErr errorType, boolean shortened)
{
	switch (errorType)
	{
		case none:
			return "No error";

		case outOfBound:
			return shortened ? "Out of bounds" : "Error: Value is out of bounds";

		case unknownComplement:
			return shortened ? "Unknowm complement" : "Error: Unknowm complement";

		case unknowmType:
			return shortened ? "Unknown request type" : "Error: Unknown request type";

		default:
			return "Unknown error";
	}
}

const uint8_t Utils::messageTypeComplementBounds (ReqMes infoType, uint8_t minMax)
{
	switch (infoType)
	{
		case RGB:
		case POW:
		case SPEED:
			return minMax == COMPLEMENT_MIN ? LIGHT_MOD_MIN : LIGHT_MOD_MAX;

		case SCO:
			return minMax == COMPLEMENT_MIN ? SOUND_COMMAND_MIN : SOUND_COMMAND_MAX;

		default:
			return minMax == COMPLEMENT_MIN ? 0 : 0;
	}
}

const uint8_t Utils::messageTypeComplementType (ReqMes infoType)
{
	switch (infoType)
	{
		case RGB:
		case POW:
		case SPEED:
			return COMPLEMENT_TYPE_LMO;

		case SCO:
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

void Utils::printHeader (Stream & stream)
{
	stream.println ("HTTP/1.1 200 OK");
	stream.println ("Content-Type: application/json");
	stream.println ("Device: ESP8266");
	stream.println (""); // Do not forget this one
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

Utils utils = Utils();