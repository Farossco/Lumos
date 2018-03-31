#include <stdlib.h>
#include <string.h>
#include "VariableChange.h"
#include "Light.h"
#include "Logger.h"
#include "Memory.h"

VariableChange::VariableChange()
{ }

void VariableChange::init ()
{
	// Initializing to default values
	changeOn  = light.isOn();
	changeRgb = light.getRgb (MOD_CONTINUOUS);
	changeMod = light.getMod();
	for (int i = MOD_MIN; i < N_MOD; i++)
		changePower[i] = light.getPower (i);
	for (int i = MOD_MIN; i < N_MOD; i++)
		changeSpeed[i] = light.getSpeed (i);
}

void VariableChange::check ()
{
	boolean flagSendInfo    = false;
	boolean flagWriteEeprom = false;

	if (changeOn != light.isOn())
	{
		Log.verbose ("\"On\" changed from %T to %T" dendl, changeOn, light.isOn());

		changeOn     = light.isOn();
		flagSendInfo = true;
	}

	if (changeRgb != light.getRgb (MOD_CONTINUOUS))
	{
		Log.verbose ("\"RGB\" of default mod changed from %X to %X" dendl, changeRgb, light.getRgb (MOD_CONTINUOUS));

		changeRgb       = light.getRgb (MOD_CONTINUOUS);
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	for (int i = MOD_MIN; i < N_MOD; i++)
	{
		if (changePower[i] != light.getPower (i))
		{
			Log.verbose ("\"Power\" of %s mod changed from %d to %d" dendl, utils.modName (i, CAPS_NONE), changePower[i], light.getPower (i));

			changePower[i]  = light.getPower (i);
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

		if (changeSpeed[i] != light.getSpeed (i))
		{
			Log.verbose ("\"Speed\" of %s mod changed from %d to %d" dendl, utils.modName (i, CAPS_NONE), changeSpeed[i], light.getSpeed (i));

			changeSpeed[i]  = light.getSpeed (i);
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}
	}

	if (changeMod != light.getMod())
	{
		Log.verbose ("\"Mod\" changed from %s (%d) to %s (%d)" dendl, utils.modName (changeMod, CAPS_NONE), changeMod, utils.modName (light.getMod(), CAPS_NONE), light.getMod());

		changeMod    = light.getMod();
		flagSendInfo = true;
	}

	if (flagSendInfo)
		sendInfo();

	if (flagWriteEeprom)
		memory.write();
} // VariableChange::check

// char varBuf[9 + 3 + 1 + (MOD_MAX < 10 ? 1 : MOD_MAX < 100 ? 2 : 1) + 6 + N_MOD * 16];
char varBuf[(3 + 1 + 1) + (3 + (MOD_MAX < 10 ? 1 : MOD_MAX < 100 ? 2 : 1) + 1) + N_MOD * (3 + 1 + 6 + 1) + N_MOD * (3 + 1 + 3 + 1) + N_MOD * (3 + 1 + 3 + 1)];

void VariableChange::sendInfo ()
{
	Log.trace ("Sending variables infos to the ESP8266" dendl);

	sprintf (varBuf, "ONF%dzMOD%dz", light.isOn(), light.getMod());

	for (int i = MOD_MIN; i < N_MOD; i++)
		sprintf (varBuf + strlen (varBuf), "RGB%d%lXz", i, light.getRgb (i));

	for (int i = MOD_MIN; i < N_MOD; i++)
		sprintf (varBuf + strlen (varBuf), "POW%d%ldz", i, utils.map (light.getPower (i), MIN_POWER, MAX_POWER, SEEKBAR_MIN, SEEKBAR_MAX));

	for (int i = MOD_MIN + 1; i < N_MOD; i++)
		sprintf (varBuf + strlen (varBuf), "SPE%d%ldz", i, i == 0 ? light.getSpeed (i) : utils.map (light.getSpeed (i), MIN_SPEED[i], MAX_SPEED[i], SEEKBAR_MIN, SEEKBAR_MAX));

	varBuf[strlen (varBuf)] = '\0';

	Log.verbose ("%s" dendl, varBuf);

	Serial1.print (varBuf);
}

VariableChange variableChange = VariableChange();
