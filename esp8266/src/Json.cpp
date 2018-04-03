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
	jsonRoot["Light"]     = jsonRootLight;
	jsonRootLight["On"]   = light.isOn();
	jsonRootLight["Mode"] = light.getMod();

	JsonArray& jsonRootLightRgb = jsonRootLight.createNestedArray ("Rgb");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		jsonRootLightRgb.add ((unsigned long) light.getRgb (i));

	JsonArray& jsonRootLightPower = jsonRootLight.createNestedArray ("Power");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		jsonRootLightPower.add ((uint8) light.getPower (i));

	JsonArray& jsonRootLightModSpeed = jsonRootLight.createNestedArray ("Speed");
	for (int i = LIGHT_MOD_MIN; i <= LIGHT_MOD_MAX; i++)
		jsonRootLightModSpeed.add ((unsigned int) light.getSpeed (i));

	// *** Sound *** //
	JsonObject& jsonRootSound = jsonBuffer.createObject();
	jsonRoot["Sound"]       = jsonRootSound;
	jsonRootSound["On"]     = sound.isOn();
	jsonRootSound["Volume"] = sound.getVolume();
	jsonRootSound["Mode"]   = sound.getMod();

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

	Log.trace (dendl);
} // Json::sendJsonToClient

Json json = Json();
