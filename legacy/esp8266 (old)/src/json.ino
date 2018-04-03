#include "esp8266.h"

void sendJsonToClient (String status, String message, WiFiClient client)
{
	DynamicJsonBuffer jsonBuffer;

	JsonObject& jsonRoot = jsonBuffer.createObject();

	jsonRoot["Status"]  = status;
	jsonRoot["Message"] = message;

	JsonObject& jsonRootDatas = jsonBuffer.createObject();
	jsonRoot["Datas"]     = jsonRootDatas;
	jsonRootDatas["On"]   = on;
	jsonRootDatas["Rgb"]  = rgb;
	jsonRootDatas["Mode"] = mode;

	JsonArray& jsonRootDatasPower = jsonRootDatas.createNestedArray ("Power");
	for (int i = MODE_MIN; i < N_MODE; i++)
		jsonRootDatasPower.add ((int) power[i]);

	JsonArray& jsonRootDatasSpeed = jsonRootDatas.createNestedArray ("Speed");
	for (int i = MODE_MIN; i < N_MODE; i++)
		jsonRootDatasSpeed.add (speed[i]);

	print ("Sending to client: ");
	if (DEBUG_ENABLED)
		jsonRoot.printTo (Serial);

	// Return the response to the client
	client.print ("HTTP/1.1 ");
	client.println ("200 OK");
	client.println ("Content-Type: application/json");
	client.println ("Device: ESP8266");
	client.println (""); // Do not forget this one
	jsonRoot.prettyPrintTo (client);
} // sendJsonToClient
