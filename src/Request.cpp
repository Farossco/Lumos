#include <TimeLib.h>
#include <string.h>
#include "Request.h"
#include "Utils.h"
#include "Logger.h"
#include "Light.h"
#include "Alarms.h"
#include "Sound.h"

#if defined(LUMOS_ARDUINO_MEGA)
# include "ArduinoSerial.h"
# include "VariableChange.h"
#endif

#if defined(LUMOS_ESP8266)
# include "ESPSerial.h"
# include "Json.h"
# include "Wifi.h"
#endif

void Request::decode (char * request, uint8_t source)
{
	int16_t requestLength = strlen (request);
	uint8_t type;
	uint8_t complement;
	int32_t information;
	int8_t error = ERR_NOE;

	if (requestLength <= 0)
		return;

	complement  = 0;
	information = 0;

	if (strstr (request, "INFO") == request)
		type = TYPE_RIF;
	else if (strstr (request, "TIME") == request)
		type = TYPE_RTM;
	else
	{
		type  = TYPE_UNK;
		error = ERR_UKR;

		for (uint8_t i = TYPE_MIN; i <= TYPE_MAX; i++)
			if (strstr (request, utils.infoTypeName (i, true)) == request)
			{
				type  = i;
				error = ERR_NOE;
				break;
			}

		// [DEBUG] Printing full word, world length and information type
		Log.verbose ("Word: %s" endl, request);
		Log.verbose ("Length: %d" endl, requestLength);
		Log.verbose ("Type: %s (%d)" endl, utils.infoTypeName (type, false), type);

		utils.reduceCharArray (&request, 3); // Remove 3 first char of the array (The prefix)

		if (utils.infoTypeComplementType (type) != COMPLEMENT_TYPE_NONE)
		{
			complement = request[0] - '0';
			utils.reduceCharArray (&request, 1); // Remove first char of the array (mod specifier)

			if (utils.infoTypeComplementType (type) == COMPLEMENT_TYPE_LMO)
			{
				Log.verbose ("Complement: %s (%d)" endl, utils.lightModName (complement, CAPS_FIRST), complement);
				if (complement < LIGHT_MOD_MIN || complement > LIGHT_MOD_MAX)
					error = ERR_UKC;
			}
			else if (utils.infoTypeComplementType (type) == COMPLEMENT_TYPE_SCP)
			{
				Log.verbose ("Command type: %s (%d)" endl, utils.soundCommandName (complement, CAPS_FIRST), complement);
				if (complement < SOUND_COMMAND_MIN || complement > SOUND_COMMAND_MAX)
					error = ERR_UKC;
			}
		}

		Log.verbose ("%s: %s" endl, utils.infoTypeName (type, false), request);

		information = strtol (request, NULL, type == TYPE_RGB ? 16 : 10);

		if (type == TYPE_RGB)
			Log.verbose ("%s (decoded): %x" endl, utils.infoTypeName (type, false), information);
		else
			Log.verbose ("%s (decoded): %l" endl, utils.infoTypeName (type, false), information);
	}

	process (type, complement, information, error, source);
} // Request::decode

void Request::process (uint8_t type, uint8_t complement, int32_t information, int8_t error, uint8_t source)
{
	if (error == ERR_NOE)
		switch (type)
		{
			case TYPE_TIM:
				if (information <= 0)
					error = ERR_OOB;
				else
					setTime (information);

				char buf[20];

				// Debug
				Log.verbose ("Time (Current value): %l" endl, now());
				Log.trace ("Time (Current value) (readable): %s" endl, utils.clock (buf));
				break;

			case TYPE_RGB:
				if (information < 0 || information > 0xFFFFFF)
					error = ERR_OOB;
				else
					light.setRgb (information, complement);

				// Debug
				Log.trace ("RGB   (Current value): %x" endl, light.getRgb (complement));
				Log.verbose ("Red   (Current value): %d" endl, light.getRed (complement));
				Log.verbose ("Green (Current value): %d" endl, light.getGreen (complement));
				Log.verbose ("Blue  (Current value): %d" dendl, light.getBlue (complement));

				break;

			case TYPE_LON:
				if (information != 0 && information != 1)
					error = ERR_OOB;
				else
				{
					if (information)
						light.switchOn();
					else
						light.switchOff();
				}

				Log.trace ("On/Off (Current value): %T" dendl, light.isOn());

				break;

			case TYPE_POW:
				if (information < SEEKBAR_MIN || information > SEEKBAR_MAX)
					error = ERR_OOB;
				else
					light.setPower (utils.map (information, SEEKBAR_MIN, SEEKBAR_MAX, LIGHT_MIN_POWER, LIGHT_MAX_POWER), complement);

				Log.trace ("Power of %s (Current value): %d" dendl, utils.lightModName (complement, CAPS_NONE), light.getPower (complement));

				break;

			case TYPE_LMO:
				if (information < LIGHT_MOD_MIN || information > LIGHT_MOD_MAX)
					error = ERR_OOB;
				else
				{
					light.setMod (information);
					light.switchOn();
				}

				Log.verbose ("Light mod (Text): %s (%d)" endl, utils.lightModName (information, CAPS_FIRST), information);
				Log.trace ("Light mod (Current value): %s (%d)" dendl, utils.lightModName (light.getMod(), CAPS_FIRST), light.getMod());

				break;

			case TYPE_SPE:
				if (LIGHT_MIN_SPEED[complement] == 0 && LIGHT_MAX_SPEED[complement] == 0)
				{
					if (information < 0)
						error = ERR_OOB;
				}
				else
				{
					if (information < SEEKBAR_MIN || information > SEEKBAR_MAX)
						error = ERR_OOB;
				}

				if (error == ERR_NOE)
					light.setSpeed (utils.map (information, SEEKBAR_MIN, SEEKBAR_MAX, LIGHT_MIN_SPEED[complement], LIGHT_MAX_SPEED[complement]), complement);

				// Debug
				Log.verbose ("Min Speed: %d" endl, LIGHT_MIN_SPEED[complement]);
				Log.verbose ("Max Speed: %d" endl, LIGHT_MAX_SPEED[complement]);
				Log.trace ("Speed of %s (Current value): %d" dendl, utils.lightModName (complement, CAPS_NONE), light.getSpeed (complement));

				break;

			case TYPE_SMO:
				if (information < SOUND_MOD_MIN || information > SOUND_MOD_MAX)
					error = ERR_OOB;
				else
					sound.setMod (information);

				Log.verbose ("Sound mod (Text): %s (%d)" endl, utils.soundModName (information, CAPS_FIRST), information);
				Log.trace ("Sound mod (Current value): %s (%d)" dendl, utils.soundModName (sound.getMod(), CAPS_FIRST), sound.getMod());

				break;

			case TYPE_VOL:
				if (information < SOUND_VOLUME_MIN || information > SOUND_VOLUME_MAX)
					error = ERR_OOB;
				else
					sound.setVolume (information);

				Log.trace ("Volume (Current value): %d" dendl, sound.getVolume());

				break;

			case TYPE_SON:
				if (information != 0 && information != 1)
					error = ERR_OOB;
				else
				{
					if (information)
						sound.switchOn();
					else
						sound.switchOff();
				}

				Log.trace ("On/Off (Current value): %T" dendl, sound.isOn());
				break;

			case TYPE_DTM:
				if (information < 0 || information > 1439)
					error = ERR_OOB;
				else
				{
					alarms.setDawnTime (information);
				}

				Log.trace ("Dawn time (Current value): %d:%d (%d)" dendl, alarms.getDawnTime() / 60, alarms.getDawnTime() % 60, alarms.getDawnTime());
				break;

			case TYPE_SCO:
				Log.verbose ("Command data: (%d)" dendl, information);

				#if defined(LUMOS_ARDUINO_MEGA)
				sound.command (complement, information);
				#endif

				break;
		}

	if (error != ERR_NOE)
	{
		Log.verbosenp (endl);
		Log.warning ("Variable has not been changed (%s)" dendl, utils.errorTypeName (error, false));
	}

	switch (source)
	{
		#if defined(LUMOS_ARDUINO_MEGA)

		case SOURCE_ARDUINO_SERIAL:
			if (type == TYPE_RTM)
			{
				Log.trace ("I don't know anything about time... Let me ask the ESP" dendl);
				serial.askForTime();
			}
			else if (type == TYPE_RIF)
			{
				variableChange.sendInfo(); // We send the variables values to the ESP8266
			}

		#endif // if defined(LUMOS_ARDUINO_MEGA)

		#if defined(LUMOS_ESP8266)

		case SOURCE_ESP8266_SERIAL:
			if (type == TYPE_RTM)
				serial.sendTime();  // We send the time to the Arduino
			else if (type == TYPE_RIF)
				Log.trace ("Info request needs to be sent by web client");
			break;

		case SOURCE_ESP8266_WEBSERVER:
			if (type == TYPE_RIF)
			{
				Log.trace ("Sending to arduino: Nothing" dendl);
				json.send ((char *) "OK", (char *) "", &wifi.client);
				return;
			}
			if (error != ERR_NOE)
			{
				Log.trace ("Sending to arduino: Nothing" dendl);
				json.send ((char *) "ERROR", (char *) utils.errorTypeName (error, true), &wifi.client);
				return;
			}

			Log.trace ("Sending to arduino: ");

			Serial.print (utils.infoTypeName (type, true));
			if (type == TYPE_RGB || type == TYPE_POW || type == TYPE_SPE || type == TYPE_SCO)
				Serial.print (complement, DEC);
			Serial.print (information, type == TYPE_RGB ? HEX : DEC);
			Serial.print ('z'); // End character

			Log.tracenp (dendl);

			json.send ((char *) "OK", (char *) "", &wifi.client);

		#endif // if defined(LUMOS_ESP8266)
	}
} // Request::process

Request request = Request();
