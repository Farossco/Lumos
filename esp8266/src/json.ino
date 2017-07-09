char * getJson (String status, String message)
{
	char buffer[2048];

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
	for (int i = 0; i <= MODE_MAX; i++)
		jsonRootDatasPower.add ((int) power[i]);

	JsonArray& jsonRootDatasSpeed = jsonRootDatas.createNestedArray ("Speed");
	for (int i = 0; i <= MODE_MAX; i++)
		jsonRootDatasSpeed.add (speed[i]);

	JsonArray& jsonRootPrayers = jsonRoot.createNestedArray ("Prayers");
	for (int i = 0; i < N_PRAYER; i++)
		jsonRootPrayers.add (prayerTime[i][2]);

	jsonRoot.printTo (buffer, sizeof(buffer));

	return buffer;
} // getJson

void sendJsonToSerial (String status, String message)
{
	char * json = getJson (status, message);

	print ("Sending to serial: ");
	Serial.print (json);
}

void sendJsonToClient (String status, String message)
{
	char * json = getJson (status, message);

	print ("Sending to client: ");
	printlnNoPrefix (json);

	// Return the response to the client
	client.print ("HTTP/1.1 ");
	client.println ("200 OK");
	client.println ("Content-Type: application/json");
	client.println ("Device: ESP8266");
	client.println (""); // Do not forget this one
	client.print (json);
}
