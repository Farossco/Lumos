JsonObject& getJson (String status, String message)
{
	JsonObject& jsonRootGlobal = jsonBuffer.createObject();
	JsonObject& datas = jsonBuffer.createObject();

	datas["On"]    = on;
	datas["RGB"]   = rgb;
	datas["Power"] = (int) power;
	datas["Mode"]  = mode;

	jsonRootGlobal["Status"]  = status;
	jsonRootGlobal["Message"] = message;
	jsonRootGlobal["Datas"]   = datas;

	return jsonRootGlobal;
}

void sendJsonToSerial (String status, String message)
{
	JsonObject& jsonRootGlobal = getJson (status, message);

	print ("Sending to serial: ");
	jsonRootGlobal.printTo (Serial);
}

void sendJsonToClient (String status, String message)
{
	JsonObject& jsonRootGlobal = getJson (status, message);

	print ("Sending to client: ");
	if (DEBUG_ENABLED)
	{
		jsonRootGlobal.printTo (Serial);
		printlnNoPrefix();
	}


	// Return the response to the client
	client.print ("HTTP/1.1 ");
	client.println ("200 OK");
	client.println ("Content-Type: application/json");
	client.println ("Device: ESP8266");
	client.println (""); // Do not forget this one
	jsonRootGlobal.printTo (client);
}
