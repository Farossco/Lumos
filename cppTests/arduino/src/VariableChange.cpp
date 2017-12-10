#include <stdlib.h>
#include <string.h>
#include "VariableChange.h"
#include "Global.h"
#include "Logger.h"

VariableChange::VariableChange()
{ }

void VariableChange::begin ()
{
	// Initializing to default values
	changeOn   = global.on;
	changeRgb  = global.rgb[MODE_DEFAULT];
	changeMode = global.mode;
	for (int i = MODE_MIN; i < N_MODE; i++)
		changePower[i] = global.power[i];
	for (int i = MODE_MIN; i < N_MODE; i++)
		changeSpeed[i] = global.speed[i];
}

void VariableChange::check ()
{
	boolean flagSendInfo    = false;
	boolean flagWriteEeprom = false;

	if (changeOn != global.on)
	{
		Log.verbose ("\"On\" changed from %T to %T" dendl, changeOn, global.on);

		changeOn        = global.on;
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (changeRgb != global.rgb[MODE_DEFAULT])
	{
		Log.verbose ("\"RGB\" of default mod changed from %X to %X" dendl, changeRgb, global.rgb[MODE_DEFAULT]);

		changeRgb       = global.rgb[MODE_DEFAULT];
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}


	for (int i = MODE_MIN; i < N_MODE; i++)
		if (changePower[i] != global.power[i])
		{
			Log.verbose ("\"Power\" of %s mod changed from %d to %d" dendl, utils.modeName (i, CAPS_NONE), changePower[i], global.power[i]);

			changePower[i] = global.power[i];
			flagSendInfo   = true;
			if (i != MODE_WAKEUP)
				flagWriteEeprom = true;
		}


	for (int i = MODE_MIN; i < N_MODE; i++)
		if (changeSpeed[i] != global.speed[i])
		{
			Log.verbose ("\"Speed\" of %s mod changed from %d to %d" dendl, utils.modeName (i, CAPS_NONE), changeSpeed[i], global.speed[i]);

			changeSpeed[i]  = global.speed[i];
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

	if (changeMode != global.mode)
	{
		Log.verbose ("\"Mod\" changed from %s (%d) to %s (%d)" dendl, utils.modeName (changeMode, CAPS_NONE), changeMode, utils.modeName (global.mode, CAPS_NONE), global.mode);

		changeMode      = global.mode;
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (flagSendInfo)
		sendInfo();

	if (flagWriteEeprom) ;
	// eepromWrite();
} // testVariableChange

void VariableChange::sendInfo ()
{
	Log.trace ("Sending variables infos to the ESP8266" dendl);

	sprintf (buf + strlen (buf), "ONF%dzMOD%dzRGB%lXz", global.on != 0, global.mode, global.rgb[MODE_DEFAULT]);

	for (int i = MODE_MIN; i < N_MODE; i++)
		sprintf (buf + strlen (buf), "POW%d%dz", i, utils.convertBoundaries (global.power[i], MIN_POWER, MAX_POWER, SEEKBAR_MIN, SEEKBAR_MAX));

	for (int i = MODE_MIN; i < N_MODE; i++)
		sprintf (buf + strlen (buf), "SPE%d%dz", i, i == 0 ? global.speed[i] : utils.convertBoundaries (global.speed[i], MIN_SPEED[i], MAX_SPEED[i], SEEKBAR_MIN, SEEKBAR_MAX));

	Log.verbose ("%s" dendl, buf);

	Serial1.print (buf);
} // sendInfo

VariableChange variableChange = VariableChange();
