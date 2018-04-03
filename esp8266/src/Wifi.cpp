#include "Wifi.h"
#include <ArduinoJson.h>
#include "Logger.h"
#include <TimeLib.h>
#include "Request.h"
#include "Json.h"

Wifi::Wifi() : server (80)
{ }

void Wifi::init ()
{
	int bufSize = 20;
	char buf[bufSize];

	WiFi.mode (WIFI_STA);

	while (!Serial)
		;

	// Connect to WiFi network
	Log.trace ("Connecting to %s" dendl, SSID0);

	WiFi.begin (SSID0, PASS0);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay (500);
		Log.tracenp (".");
	}
	Log.tracenp (endl);

	Log.trace ("WiFi connected" dendl);

	// Start the server
	server.begin();

	WiFi.localIP().toString().toCharArray (buf, bufSize);

	Log.trace ("Server started" dendl);
	Log.trace ("Local IP: %s" dendl, buf);
}

void Wifi::getTime ()
{
	DynamicJsonBuffer jsonBuffer;
	WiFiClient client;
	const int lineSize = 200;
	char lineArray[lineSize];
	char * line = lineArray;
	unsigned long timeout;
	int code;
	const char * json, * status, * timestamp;

	timestamp = 0;
	status    = "";

	Log.trace ("Connecting to %s" dendl, TIME_HOST);

	// Use WiFiClient class to create TCP connections
	if (!client.connect (TIME_HOST, TIME_HTTP_PORT))
	{
		Log.error ("Connection failed" dendl);
		return;
	}

	// We now create an url for the request
	const char url[] = "/v2/get-time-zone?format=" TIME_FORMAT "&key=" TIME_KEY "&by=" TIME_BY "&zone=" TIME_ZONE "&fields=" TIME_FIELDS;

	// [DEBUG] Printing the url
	Log.trace ("Requesting URL: %s" dendl, url);

	// Sending web request
	client.print (String ("GET ") + url + " HTTP/1.1\r\n" + "Host: " + TIME_HOST + "\r\n" + "Connection: close\r\n\r\n");
	timeout = millis();

	// Waiting for an answer
	while (!client.available())
	{
		// If nothing comes out after the timeout, we abort
		if (millis() - timeout > TIME_REQUEST_TIMEOUT)
		{
			Log.error ("Client Timeout !" dendl);
			client.stop();
			return;
		}
	}

	Log.verbose ("Server answer:" endl);
	Log.verbose ("<======================================== Start ========================================>" dendl);

	// Read all the lines of the reply from server and print them to Serial
	while (client.available())
	{
		size_t length = client.readBytesUntil ('\r', line, lineSize);
		line[length] = '\0';

		Log.verbose ("%s" endl, line[0] == '\n' || line[0] == '\r' ? line + 1 : line); // [DEBUG] We print the line we're currently reading
		if (strstr (line, "{\"status\"") == line + 1)
			break;
	}
	Log.verbose ("<========================================= End =========================================>" dendl);

	if (strstr (line, "{\"status\"") == line + 1)
	{
		Log.trace ("Success !" dendl);
		// The first charactere is a nl, so we don't want it
		line++;
	}
	else
	{
		Log.error ("Failed ! (%d)" dendl, strstr (line, "{\"status\"") - line);
	}

	Log.trace ("Closing connection" dendl);

	// At this point, the last line of the answer is in the line variable,
	// that's actually the one we want

	Log.verbose ("Json: %s" dendl, line);

	JsonObject& jsonRoot = jsonBuffer.parseObject (line);
	status    = jsonRoot["status"];
	timestamp = jsonRoot["timestamp"];

	Log.trace ("Status: %s" dendl, status);

	if (status[0] != 'O' || status[1] != 'K')
	{
		Log.error ("Wrong status, leaving." dendl);
		return;
	}

	Log.trace ("Timestamp: %s" dendl, timestamp);

	Log.trace ("Setting Time to: %l" dendl, strtol (timestamp, NULL, 10));

	setTime (strtol (timestamp, NULL, 10));

	Log.trace ("Time set!" dendl);
} // Wifi::getTime

void Wifi::receiveAndDecode ()
{
	WiFiClient client = server.available();
	const int bufSize = 50;
	char bufArray[bufSize];
	char * buf              = bufArray;
	const int ipAddressSize = 20;
	char ipAddress[ipAddressSize];
	uint8_t type = TYPE_UNK;
	uint8_t complement;
	int32_t information;
	int8_t error;

	uint8_t length;

	if (!client) return;  // If nobody connected, we stop here

	while (!client.available()) // Wait until the client sends some data
		delay (1);

	client.remoteIP().toString().toCharArray (ipAddress, ipAddressSize);
	Log.trace ("Received request from %s" dendl, ipAddress);

	length      = client.readBytesUntil ('\r', buf, bufSize);
	buf[length] = '\0';
	Log.trace ("Request: %s" dendl, buf);

	// Remove unwanted characteres (GET /)
	utils.reduceCharArray (&buf, 5);

	*strstr (buf, " ") = '\0'; // Terminating the array at the first space

	if (strstr (buf, "favicon.ico") == buf)
	{
		Log.trace ("Favicon request, ignoring...");
		return;
	}

	request.decode (buf, type, complement, information, error);

	if (error == TYPE_RIF)
	{
		Log.trace ("Sending to arduino: Nothing" dendl);
		json.send ("OK", "", &client);
		client.flush();
		client.stop();
		return;
	}
	if (error != ERR_NOE)
	{
		Log.trace ("Sending to arduino: Nothing" dendl);
		json.send ("ERROR", (char *) utils.errorTypeName (error, true), &client);
		client.flush();
		client.stop();
		return;
	}

	Log.trace ("Sending to arduino: ");

	Serial.print (utils.infoTypeName (type, true));
	if (type == TYPE_RGB || type == TYPE_POW || type == TYPE_SPE)
		Serial.print (complement, DEC);
	Serial.print (information, type == TYPE_RGB ? HEX : DEC);
	Serial.print ('z'); // End character

	Log.tracenp (dendl);

	json.send ("OK", "", &client);

	client.flush();
	client.stop();
} // Wifi::receiveAndDecode

Wifi wifi = Wifi();
