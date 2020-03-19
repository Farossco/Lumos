#include <Arduino.h>

#if defined(LUMOS_ESP8266)

# include "Wifi.h"
# include <ArduinoJson.h>
# include "Logger.h"
# include <TimeLib.h>
# include "Request.h"
# include "Json.h"
# include "Utils.h"

Wifi::Wifi() : server (80)
{ }

void Wifi::init ()
{
	const int bufSize = 20;
	char buf[bufSize];

	WiFi.mode (WIFI_STA);

	// Connect to WiFi network
	Log.trace ("Connecting to %s", SSID0);

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

	if (Log.isEnabledFor (LEVEL_TRACE))
		WiFi.localIP().toString().toCharArray (buf, bufSize);

	Log.trace ("Server started" dendl);
	Log.trace ("Local IP: %s" dendl, buf);

	restartTimeout = millis();
}

void Wifi::getTime ()
{
	const size_t capacity =
	  JSON_OBJECT_SIZE (3) // root (status, message, timestamp)
	  + 50;                // String duplications + security margin
	DynamicJsonDocument doc (capacity);

	WiFiClient client;
	const int lineSize = 200;
	char line[lineSize];
	char * pLine     = line; // we create a pointer that we can increment
	const char url[] = "/v2.1/get-time-zone?format=" TIME_FORMAT "&key=" TIME_KEY "&by=" TIME_BY "&zone=" TIME_ZONE "&fields=" TIME_FIELDS;
	const char * status;
	unsigned long timeout, timestamp;

	Log.trace ("Connecting to %s" dendl, TIME_HOST);

	// Use WiFiClient class to create TCP connections
	if (!client.connect (TIME_HOST, TIME_HTTP_PORT))
	{
		Log.error ("Connection failed" dendl);
		return;
	}

	// We now create an url for the request

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
	Log.verbose ("<======================================== Start ========================================>" endl);

	// Read all the lines of the reply from server and print them to Serial
	while (client.available())
	{
		size_t length = client.readBytesUntil ('\r', pLine, lineSize);
		pLine[length] = '\0';

		Log.verbose ("%s" endl, pLine[0] == '\n' || pLine[0] == '\r' ? pLine + 1 : pLine); // [DEBUG] We print the line we're currently reading
		if (strstr (pLine, "{\"status\"") == pLine + 1)
			break;
	}
	Log.verbose ("<========================================= End =========================================>" dendl);

	if (strstr (pLine, "{\"status\"") == pLine + 1)
	{
		Log.trace ("Success !" dendl);

		pLine++; // The first charactere is a nl, so we don't want it
	}
	else
	{
		Log.error ("Failed ! (%d)" dendl, strstr (pLine, "{\"status\"") - pLine);
	}

	Log.trace ("Closing connection" dendl);

	// At this point, the json of the answer is in the pline variable,
	// that's actually the one we want

	Log.verbose ("Json: \"%s\"" dendl, pLine);

	// Deserialize the JSON document
	deserializeJson (doc, pLine);

	status    = doc["status"];
	timestamp = doc["timestamp"];

	Log.trace ("Status: %s" dendl, status);

	if (status[0] != 'O' || status[1] != 'K')
	{
		Log.error ("Wrong status, leaving." dendl);
		return;
	}

	Log.trace ("Timestamp: %l" dendl, timestamp);

	Log.trace ("Setting Time to: %l" dendl, timestamp);

	setTime (timestamp);

	Log.trace ("Time set!" dendl);
} // Wifi::getTime

void Wifi::receiveAndDecode ()
{
	WiFiClient client = server.available();

	if (!client.connected()) return;  // If nobody connected, we stop here

	String str;

	Log.trace ("Received request from %s" dendl, client.remoteIP().toString().c_str());

	Log.verbose ("Waiting for client data");

	clientTimeout = millis();
	while (!client.available()) // Wait until the client sends some data
	{
		if (Log.isEnabledFor (LEVEL_VERBOSE))
			delay (10);
		else
			delay (1);

		Log.verbosenp (".");

		if (millis() - clientTimeout > 1000)
		{
			Log.verbosenp (dendl);
			Log.warning ("Client data timed out" dendl);
			client.stop();

			return;
		}
	}

	Log.verbosenp (dendl);

	str = client.readStringUntil ('\r');
	client.flush();

	if (!str.startsWith ("GET"))
	{
		Log.warning ("Not a GET request, ignoring" dendl);
		utils.printHeader (client);
		client.print ("Use GET instead");
		delay (1);
		return;
	}

	if (str.startsWith ("GET /favicon.ico"))
	{
		Log.verbose ("Favicon request, ignoring..." dendl);
		utils.printHeader (client);
		client.print ("No favicon");
		delay (1);
		return;
	}

	Log.trace ("Request: |%s|" dendl, str.c_str());

	str = str.substring (5, str.indexOf (' ', 5)); // Trimming the string from the end of "GET /" to where we meet the first space

	Req requestData = request.decode (str);

	if (requestData.type == requestInfos)
	{
		Log.trace ("Sending to arduino: Nothing" dendl);
		json.send ((char *) "OK", (char *) "", &client);

		delay (1);

		return;
	}
	if (requestData.error != none)
	{
		Log.trace ("Sending to arduino: Nothing" dendl);
		json.send ((char *) "ERROR", (char *) utils.errorTypeName (requestData.error, true), &client);

		delay (1);

		return;
	}

	String data = utils.messageTypeName (requestData.type, true);
	if (utils.messageTypeComplementType (requestData.type) != COMPLEMENT_TYPE_NONE) data += requestData.complement;
	data += utils.ltos (requestData.information, requestData.type == RGB ? HEX : DEC);
	data += 'z';

	Log.trace ("Sending to arduino: %s" dendl, data.c_str());
	Serial1.print (data);

	json.send ((char *) "OK", (char *) "", &client);

	delay (1);
} // Wifi::receiveAndDecode

Wifi wifi = Wifi();

#endif // if defined(LUMOS_ESP8266)
