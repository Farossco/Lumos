#include "Json.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include "Light.h"
#include "Sound.h"
#include "Logger.h"

Json::Json()
{ }

void Json::send (char * status, char * message, WiFiClient * client)
{
	DynamicJsonBuffer jsonBuffer;

	JsonObject& jsonRoot = jsonBuffer.createObject();

	jsonRoot["Status"]  = status;
	jsonRoot["Message"] = message;

	// *** Light *** //
	JsonObject& jsonRootLight = jsonBuffer.createObject();
	jsonRoot["Light"]    = jsonRootLight;
	jsonRootLight["On"]  = light.isOn();
	jsonRootLight["Mod"] = light.getMod();

	JsonArray& jsonRootLightRgb = jsonRootLight.createNestedArray ("Rgb");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		jsonRootLightRgb.add ((unsigned long) light.getRgb (i));

	JsonArray& jsonRootLightPower = jsonRootLight.createNestedArray ("Power");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		jsonRootLightPower.add ((uint8_t) utils.map (light.getPower (i), LIGHT_MIN_POWER, LIGHT_MAX_POWER, SEEKBAR_MIN, SEEKBAR_MAX));

	JsonArray& jsonRootLightModSpeed = jsonRootLight.createNestedArray ("Speed");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		jsonRootLightModSpeed.add ((unsigned int) utils.map (light.getSpeed (i), LIGHT_MIN_SPEED[i], LIGHT_MAX_SPEED[i], SEEKBAR_MIN, SEEKBAR_MAX));

	// *** Sound *** //
	JsonObject& jsonRootSound = jsonBuffer.createObject();
	jsonRoot["Sound"]       = jsonRootSound;
	jsonRootSound["On"]     = sound.isOn();
	jsonRootSound["Volume"] = sound.getVolume();
	jsonRootSound["Mod"]    = sound.getMod();

	Log.trace ("Sending to client: ");
	if (SERIAL_LOG_ENABLED)
		jsonRoot.printTo (Serial);

	// Return the response to the client
	client->print ("HTTP/1.1 ");
	client->println ("200 OK");
	client->println ("Content-Type: application/json");
	client->println ("Device: ESP8266");
	client->println (""); // Do not forget this one
	jsonRoot.prettyPrintTo (*client);

	Log.tracenp (dendl);
} // Json::sendJsonToClient

Json json = Json();
