#if defined(__AVR_ATmega2560__)

#include <stdlib.h>
#include <string.h>
#include "VariableChange.h"
#include "Light.h"
#include "Logger.h"
#include "Memory.h"
#include "Sound.h"
#include "Request.h"
#include "Utils.h"
#include "Alarms.h"

VariableChange::VariableChange()
{ }

void VariableChange::init ()
{
	// Initializing to default values
	changeOn       = light.isOn();
	changeRgb      = light.getRgb (LIGHT_MOD_CONTINUOUS);
	changeLightMod = light.getMod();
	changeSoundMod = sound.getMod();
	for (uint8_t i = LIGHT_MOD_MIN; i < LIGHT_N_MOD; i++)
		changePower[i] = light.getPower (i);
	for (uint8_t i = LIGHT_MOD_MIN; i < LIGHT_N_MOD; i++)
		changeSpeed[i] = light.getSpeed (i);
	changeDawnTime = alarms.getDawnTime();
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

	if (changeRgb != light.getRgb (LIGHT_MOD_CONTINUOUS))
	{
		Log.verbose ("\"RGB\" of default mod changed from %X to %X" dendl, changeRgb, light.getRgb (LIGHT_MOD_CONTINUOUS));

		changeRgb       = light.getRgb (LIGHT_MOD_CONTINUOUS);
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	for (uint8_t i = LIGHT_MOD_MIN; i < LIGHT_N_MOD; i++)
	{
		if (changePower[i] != light.getPower (i))
		{
			Log.verbose ("\"Power\" of %s mod changed from %d to %d" dendl, utils.lightModName (i, CAPS_NONE), changePower[i], light.getPower (i));

			changePower[i]  = light.getPower (i);
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}

		if (changeSpeed[i] != light.getSpeed (i))
		{
			Log.verbose ("\"Speed\" of %s mod changed from %d to %d" dendl, utils.lightModName (i, CAPS_NONE), changeSpeed[i], light.getSpeed (i));

			changeSpeed[i]  = light.getSpeed (i);
			flagSendInfo    = true;
			flagWriteEeprom = true;
		}
	}

	if (changeLightMod != light.getMod())
	{
		Log.verbose ("\"Light mod\" changed from %s (%d) to %s (%d)" dendl, utils.lightModName (changeLightMod, CAPS_NONE), changeLightMod, utils.lightModName (light.getMod(), CAPS_NONE), light.getMod());

		changeLightMod  = light.getMod();
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (changeSoundMod != sound.getMod())
	{
		Log.verbose ("\"Sound mod\" changed from %s (%d) to %s (%d)" dendl, utils.soundModName (changeSoundMod, CAPS_NONE), changeSoundMod, utils.soundModName (sound.getMod(), CAPS_NONE), sound.getMod());

		changeSoundMod  = sound.getMod();
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (changeDawnTime != alarms.getDawnTime())
	{
		Log.verbose ("\"Dawn time\" changed from %d:%d (%d) to %d:%d (%d)" dendl, changeDawnTime / 60, changeDawnTime % 60, changeDawnTime, alarms.getDawnTime() / 60, alarms.getDawnTime() % 60, alarms.getDawnTime());

		changeDawnTime  = alarms.getDawnTime();
		flagSendInfo    = true;
		flagWriteEeprom = true;
	}

	if (flagSendInfo)
		sendInfo();

	if (flagWriteEeprom)
		memory.writeForAll();
} // VariableChange::check

void VariableChange::sendInfo ()
{
	Log.trace ("Sending variables infos to the ESP8266" dendl);
	Log.verbose (""); // Printing prefix once before entering the loop

	for (uint8_t i = TYPE_SEND_MIN; i <= TYPE_SEND_MAX; i++)
	{
		for (uint8_t j = utils.infoTypeComplementBounds (i, COMPLEMENT_MIN); j <= utils.infoTypeComplementBounds (i, COMPLEMENT_MAX); j++)
		{
			char information[15] = "\n";

			sprintf (information, "%s", utils.infoTypeName (i, true)); // Prefix

			switch (i) // info
			{
				case TYPE_RGB:
					sprintf (information + strlen (information), "%d%lX", j, light.getRgb (j));
					break;

				case TYPE_LON:
					sprintf (information + strlen (information), "%d", light.isOn());
					break;

				case TYPE_POW:
					sprintf (information + strlen (information), "%d%d", j, (uint8_t) utils.map (light.getPower (j), LIGHT_MIN_POWER, LIGHT_MAX_POWER, SEEKBAR_MIN, SEEKBAR_MAX));
					break;

				case TYPE_LMO:
					sprintf (information + strlen (information), "%d", light.getMod());
					break;

				case TYPE_SPE:
					sprintf (information + strlen (information), "%d%d", j, (uint16_t) utils.map (light.getSpeed (j), LIGHT_MIN_SPEED[j], LIGHT_MAX_SPEED[j], SEEKBAR_MIN, SEEKBAR_MAX));
					break;

				case TYPE_SMO:
					sprintf (information + strlen (information), "%d", sound.getMod());
					break;

				case TYPE_VOL:
					sprintf (information + strlen (information), "%d", sound.getVolume());
					break;

				case TYPE_SON:
					sprintf (information + strlen (information), "%d", sound.isOn());
					break;

				case TYPE_DTM:
					sprintf (information + strlen (information), "%d", alarms.getDawnTime());
			}
			sprintf (information + strlen (information), "z"); // Suffix

			Log.verbosenp ("%s", information);
			Serial1.print (information);
		}
	}

	Log.verbosenp (dendl);
} // VariableChange::sendInfo

VariableChange variableChange = VariableChange();

#endif // if defined(__AVR_ATmega2560__)
