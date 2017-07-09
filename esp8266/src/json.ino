char * getJson (String status, String message)
{
	char buffer[2048];

	DynamicJsonBuffer jsonBuffer;

	JsonObject& jsonRootGlobal = jsonBuffer.createObject();
	JsonObject& datas     = jsonBuffer.createObject();
	JsonArray& datasRGB   = datas.createNestedArray ("RGB");
	JsonArray& datasPower = datas.createNestedArray ("Power");
	JsonArray& datasSpeed = datas.createNestedArray ("Speed");

	datas["On"] = on;
	for (int i = 0; i <= MODE_MAX; i++)
		datasRGB.add (rgb[i]);
	for (int i = 0; i <= MODE_MAX; i++)
		datasPower.add ((int) power[i]);
	for (int i = 0; i <= MODE_MAX; i++)
		datasSpeed.add (speed[i]);
	datas["Mode"] = mode;

	jsonRootGlobal["Status"]  = status;
	jsonRootGlobal["Message"] = message;
	jsonRootGlobal["Datas"]   = datas;

	jsonRootGlobal.printTo (buffer, sizeof(buffer));

	return buffer;
}

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
