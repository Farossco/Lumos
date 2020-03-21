#include <Arduino.h>

#if defined(LUMOS_ESP8266)

# include "Json.h"
# include <ArduinoJson.h>
# include "Light.h"
# include "Sound.h"
# include "Logger.h"
# include "Utils.h"

Json::Json()
{ }

void Json::send (const char * status, const char * message, Stream * stream, bool printHeader)
{
	const size_t capacity =
	  JSON_OBJECT_SIZE (4)                 // root (status/message/light/sound)
	  + strlen (status) + strlen (message) // status + message (Strings)
	  + JSON_OBJECT_SIZE (5)               // light (on/mod/rgb/power/speed)
	  + 3 * JSON_ARRAY_SIZE (LIGHT_N_MOD)  // light:rgb[] + light:power[] + light:speed[]
	  + JSON_OBJECT_SIZE (3)               // sound (on/volume/mod)
	  + 10;                                // Security margin

	DynamicJsonDocument root (capacity);

	root["Status"]  = status;
	root["Message"] = message;

	// *** Light *** //
	JsonObject rootLight = root.createNestedObject ("Light");
	rootLight["On"]  = light.isOn();
	rootLight["Mod"] = light.getMod();

	JsonArray rootLightRgb = rootLight.createNestedArray ("Rgb");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		rootLightRgb.add ((unsigned long) light.getRgb (i));

	JsonArray rootLightPower = rootLight.createNestedArray ("Power");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		rootLightPower.add ((uint8_t) utils.map (light.getPower (i), LIGHT_MIN_POWER, LIGHT_MAX_POWER, SEEKBAR_MIN, SEEKBAR_MAX));

	JsonArray rootLightSpeed = rootLight.createNestedArray ("Speed");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		rootLightSpeed.add ((unsigned int) utils.map (light.getSpeed (i), LIGHT_MIN_SPEED[i], LIGHT_MAX_SPEED[i], SEEKBAR_MIN, SEEKBAR_MAX));

	// *** Sound *** //
	JsonObject rootSound = root.createNestedObject ("Sound");
	rootSound["On"]     = sound.isOn();
	rootSound["Volume"] = sound.getVolume();
	rootSound["Mod"]    = sound.getMod();

	Log.trace ("Sending to client: ");
	if (Log.isEnabledFor (LEVEL_TRACE))
		serializeJson (root, Serial);

	// Return the response to the client
	utils.printHeader (*stream);
	serializeJson (root, *stream);

	Log.tracenp (dendl);
} // send

Json json = Json();

#endif // if defined(LUMOS_ESP8266)