#include <Arduino.h>

#include "Json.h"
#include <ArduinoJson.h>
#include "Light.h"
#include "Sound.h"
#include "ArduinoLogger.h"
#include "Utils.h"
#include "Resources.h"
#include "Alarms.h"

Json::Json()
{}

String Json::getDataPretty()
{
	String string;

	generateData(string, true);

	return string;
}

String Json::getData()
{
	String string;

	generateData(string, false);

	return string;
}

/* a
 * n
 * k
 * l */
void Json::generateData(String & string,
  bool                         pretty)
{
	const size_t capacity = 0             /* Necessary margin */
	  + JSON_OBJECT_SIZE(5)               /* root (status/message/light/sound/alarms) */
	  + JSON_OBJECT_SIZE(5)               /* light (on/mode/rgb/power/speed) */
	  + 3 * JSON_ARRAY_SIZE(LightMode::N) /* light:rgb[] + light:power[] + light:speed[] */
	  + JSON_OBJECT_SIZE(3)               /* sound (on/volume/mode) */
	  + JSON_OBJECT_SIZE(2)               /* alarms (dawn/sunset) */
	  + JSON_OBJECT_SIZE(3)               /* dawn (volume/time/duration/) */
	  + JSON_OBJECT_SIZE(2)               /* sunset (duration/decreaseTime) */
	;

	StaticJsonDocument<capacity> root;

	root["Status"]  = "OK";
	root["Message"] = "";

	/* ****** Light ****** // */
	JsonObject rootLight = root.createNestedObject("Light");
	rootLight["On"]   = light.isOn();             /* -- On/Off -- // */
	rootLight["Mode"] = (uint8_t)light.getMode(); /* -- Mode -- // */

	/* -- Rgb -- // */
	JsonArray rootLightRgb = rootLight.createNestedArray("Rgb");
	for (LightMode mode; mode < LightMode::N; mode++)
		rootLightRgb.add(light.getRgb(mode).value());

	/* -- Power -- // */
	JsonArray rootLightPower = rootLight.createNestedArray("Power");
	for (LightMode mode; mode < LightMode::N; mode++)
		rootLightPower.add(light.getPowerPercent(mode).value());

	/* -- Speed -- // */
	JsonArray rootLightSpeed = rootLight.createNestedArray("Speed");
	for (LightMode mode; mode < LightMode::N; mode++)
		rootLightSpeed.add(light.getSpeedPercent(mode).value());


	/* ****** Sound ****** // */
	JsonObject rootSound = root.createNestedObject("Sound");
	rootSound["On"]     = sound.isOn();              /* -- On/Off -- // */
	rootSound["Volume"] = sound.getVolume().value(); /* -- Volume -- // */
	rootSound["Mode"]   = (uint8_t)sound.getMode();  /* -- Mode -- // */


	/* ****** Alarms ****** // */
	JsonObject rootAlarms = root.createNestedObject("Alarms");

	/* -- Dawn -- // */
	JsonObject rootAlarmsDawn = rootAlarms.createNestedObject("Dawn");
	rootAlarmsDawn["Volume"]   = alarms.getDawnVolume().value();   /* Volume // */
	rootAlarmsDawn["Time"]     = alarms.getDawnTime().value();     /* Time // */
	rootAlarmsDawn["Duration"] = alarms.getDawnDuration().value(); /* Duration // */

	/* -- Sunset -- // */
	JsonObject rootAlarmsSunset = rootAlarms.createNestedObject("Sunset");
	rootAlarmsSunset["Duration"]     = alarms.getSunsetDuration().value();     /* Time // */
	rootAlarmsSunset["DecreaseTime"] = alarms.getSunsetDecreaseTime().value(); /* Duration // */

	if (pretty)
		serializeJsonPretty(root, string);
	else
		serializeJson(root, string);
} /* Json::generateData */

String Json::getResourcesPretty()
{
	String string;

	generateResources(string, true);

	return string;
}

String Json::getResources()
{
	String string;

	generateResources(string, false);

	return string;
}

void Json::generateResources(String & string, bool pretty)
{
	const int colorNRows    = sizeof(WebcolorList) / sizeof(*WebcolorList);
	const int colorNColumns = sizeof(*WebcolorList) / sizeof(**WebcolorList);

	const size_t capacity = 0
	  + JSON_OBJECT_SIZE(3)                         /* root (Status/Message/Light) */
	  + JSON_OBJECT_SIZE(2)                         /* light (ModeNames/Colors) */
	  + JSON_ARRAY_SIZE(LightMode::N)               /* modeNames array */
	  + LightMode::N * JSON_STRING_SIZE(15)         /* modeNames Strings */
	  + JSON_ARRAY_SIZE(colorNRows)                 /* color array */
	  + colorNRows * JSON_ARRAY_SIZE(colorNColumns) /* color[i] array */
	;

	StaticJsonDocument<capacity> root;

	root["Status"]  = "OK";
	root["Message"] = "";

	/* ****** Light ****** // */
	JsonObject rootLight = root.createNestedObject("Light");

	/* -- ModeNames -- // */
	JsonArray rootLightModeNames = rootLight.createNestedArray("ModeNames");
	for (LightMode mode; mode <= LightMode::N; mode++)
		rootLightModeNames.add(mode.toString());

	/* -- Colors -- // */
	JsonArray rootLightColors = rootLight.createNestedArray("Colors");
	for (int i = 0; i < colorNRows; i++) {
		JsonArray rootLightColorsElement = rootLightColors.createNestedArray();

		for (int j = 0; j < colorNColumns; j++)
			rootLightColorsElement.add(WebcolorList[i][j]);
	}

	if (pretty)
		serializeJsonPretty(root, string);
	else
		serializeJson(root, string);
} /* Json::generateResources */

String Json::getErrorPretty(RequestError error)
{
	String string;

	generateError(string, true, error);

	return string;
}

String Json::getError(RequestError error)
{
	String string;

	generateError(string, false, error);

	return string;
}

void Json::generateError(String & string, bool pretty, RequestError error)
{
	const size_t capacity = 0
	  + JSON_OBJECT_SIZE(2)   /* root (status/message) */
	  + JSON_STRING_SIZE(100) /* message (String duplication) */
	;

	StaticJsonDocument<capacity> root;

	root["Status"]  = "ERROR";
	root["Message"] = error.toString();

	if (pretty)
		serializeJsonPretty(root, string);
	else
		serializeJson(root, string);
}

Json json = Json();
