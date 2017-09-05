#include "esp8266.h"

void readWeb ()
{
	WiFiClient client = server.available(); // Check if a client has connected
	long result;
	int infoMode, infoType, errorType;

	if (!client) return;  // If nobody connected, we stop here

	while (!client.available()) // Wait until the client sends some data
		delay (1);

	decodeRequest (decodeWeb (client), &result, &infoMode, &infoType, &errorType);

	if (infoType == TYPE_RIF)
	{
		println ("Sending to arduino: Nothing");
		sendJsonToClient ("OK", "", client);
		client.flush();
		client.stop();
		return;
	}

	if (errorType != ERR_NOE)
	{
		println ("Sending to arduino: Nothing");
		sendJsonToClient ("ERROR", errorTypeName (errorType, true), client);
		client.flush();
		client.stop();
		return;
	}

	print ("Sending to arduino: ");

	Serial.print (infoTypeName (infoType, true));

	if (infoType == TYPE_RGB || infoType == TYPE_POW || infoType == TYPE_SPE)
		Serial.print (infoMode, DEC);

	Serial.print (result, infoType == TYPE_RGB ? HEX : DEC);
	Serial.print ('z'); // End character

	printlnNoPrefix();

	sendJsonToClient ("OK", "", client);

	client.flush();
	client.stop();
} // readWeb

String decodeWeb (WiFiClient client)
{
	String request;

	request = client.readStringUntil ('\r'); // Read the first line of the request

	// [DEBUG] Printing client IP
	printlnNoPrefix();
	print ("Received request from ");
	printlnNoPrefix (client.remoteIP());

	// [DEBUG] Printing the incomming request
	println ("Request: " + request);

	// Remove unwanted characteres
	request.remove (0, 5);
	request.remove (request.indexOf (" "), request.length() - request.indexOf (" "));

	return request;
}

void initWifiServer ()
{
	WiFi.mode(WIFI_AP);
	
	// Connect to WiFi network
	printlnNoPrefix();
	print ("Connecting to ");
	printNoPrefix (SSID0);

	WiFi.begin (SSID0, PASS0);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay (500);
		printNoPrefix (".");
	}

	printlnNoPrefix();
	println ("WiFi connected");

	// Start the server
	server.begin();

	println ("Server started");
}
