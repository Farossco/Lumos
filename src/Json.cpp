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

String Json::getDataPretty (const char * status, const char * message)
{
	String string;

	generateData (string, status, message, true);

	return string;
}

String Json::getData (const char * status, const char * message)
{
	String string;

	generateData (string, status, message, false);

	return string;
}

void Json::generateData (String & string, const char * status, const char * message, bool pretty)
{
	const size_t capacity =
	  JSON_OBJECT_SIZE (4)                 // root (status/message/light/sound)
	  + strlen (status) + strlen (message) // status + message (Strings)
	  + JSON_OBJECT_SIZE (5)               // light (on/mode/rgb/power/speed)
	  + 3 * JSON_ARRAY_SIZE (LIGHT_MOD_N)  // light:rgb[] + light:power[] + light:speed[]
	  + JSON_OBJECT_SIZE (3)               // sound (on/volume/mode)
	  + 10;                                // Security margin

	DynamicJsonDocument root (capacity);

	root["Status"]  = status;
	root["Message"] = message;

	// ****** Light ****** //
	JsonObject rootLight = root.createNestedObject ("Light");
	rootLight["On"]   = light.isOn();
	rootLight["Mode"] = light.getMode();

	// -- Rgb -- //
	JsonArray rootLightRgb = rootLight.createNestedArray ("Rgb");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		rootLightRgb.add ((unsigned long) light.getRgb (i));

	// -- Power -- //
	JsonArray rootLightPower = rootLight.createNestedArray ("Power");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		rootLightPower.add (light.getPowerPercent (i));

	// -- Speed -- //
	JsonArray rootLightSpeed = rootLight.createNestedArray ("Speed");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		rootLightSpeed.add ((unsigned int) light.getSpeedPercent (i));

	// ****** Sound ****** //
	JsonObject rootSound = root.createNestedObject ("Sound");
	rootSound["On"]     = sound.isOn();
	rootSound["Volume"] = sound.getVolume();
	rootSound["Mode"]   = sound.getMode();

	if (pretty)
		serializeJsonPretty (root, string);
	else
		serializeJson (root, string);
} // send

String Json::getResourcesPretty ()
{
	String string;

	generateResources (string, true);

	return string;
}

String Json::getResources ()
{
	String string;

	generateResources (string, false);

	return string;
}

void Json::generateResources (String & string, bool pretty)
{
	const int colorNRows    = sizeof(colorList) / sizeof(*colorList);
	const int colorNColumns = sizeof(*colorList) / sizeof(**colorList);

	const size_t capacity = 0
	  + JSON_OBJECT_SIZE (3)                         // root (Status/Message/Light)
	  + JSON_OBJECT_SIZE (2)                         // light (ModeNames/Colors)
	  + JSON_ARRAY_SIZE (LIGHT_MOD_N)                // modeNames array
	  + LIGHT_MOD_N * 15                             // modeNames Strings
	  + (JSON_ARRAY_SIZE (colorNRows))               // color array
	  + colorNRows * JSON_ARRAY_SIZE (colorNColumns) // color[i] array
	;
	DynamicJsonDocument root (capacity);

	root["Status"]  = "OK";
	root["Message"] = "";

	// ****** Light ****** //
	JsonObject rootLight = root.createNestedObject ("Light");

	// -- ModeNames -- //
	JsonArray rootLightModeNames = rootLight.createNestedArray ("ModeNames");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		rootLightModeNames.add (lightModeName[i]);

	// -- Colors -- //
	JsonArray rootLightColors = rootLight.createNestedArray ("Colors");
	for (int i = 0; i < colorNRows; i++)
	{
		JsonArray rootLightColorsElement = rootLightColors.createNestedArray();

		for (int j = 0; j < colorNColumns; j++)
			rootLightColorsElement.add (colorList[i][j]);
	}

	if (pretty)
		serializeJsonPretty (root, string);
	else
		serializeJson (root, string);
} // send

Json json = Json();

#endif // if defined(LUMOS_ESP8266)