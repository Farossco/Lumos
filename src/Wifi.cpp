#include <Arduino.h>

#if defined(LUMOS_ESP8266)

# include "Wifi.h"
# include <ArduinoJson.h>
# include "Logger.h"
# include <TimeLib.h>
# include "Request.h"
# include "Json.h"
# include "Utils.h"

Wifi::Wifi() : server (80), list()
{ }

void Wifi::init ()
{
	WiFi.mode (WIFI_STA);

	Log.trace ("Connecting to " SSID0);

	WiFi.begin (SSID0, PASS0);

	time_t a = millis();
	while (WiFi.status() != WL_CONNECTED)
	{
		if (millis() - a >= 500)
		{
			Log.tracenp (".");
			a = millis();
		}
		delay (1);
	}
	Log.tracenp (endl);

	Log.trace ("WiFi connected" dendl);

	server.begin();

	Log.trace ("Server started" dendl);
	Log.trace ("Local IP: %s" dendl, WiFi.localIP().toString().c_str());

	restartTimeout = millis();
}

void Wifi::getTime ()
{
	const size_t capacity =
	  JSON_OBJECT_SIZE (3) // root (status, message, timestamp)
	  + 50;                // String duplications + security margin
	DynamicJsonDocument doc (capacity);

	WiFiClient client;
	String line, status;
	const char url[] = "/v2.1/get-time-zone?format=" TIME_FORMAT "&key=" TIME_KEY "&by=" TIME_BY "&zone=" TIME_ZONE "&fields=" TIME_FIELDS;
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
	while (client.available() && line.indexOf ("{\"status\"") == -1)
	{
		line = client.readStringUntil ('\r');
		Log.verbosenp ("%s" endl, line.charAt (0) == '\n' ? line.c_str() + 1 : line.c_str()); // [DEBUG] We print the line we're currently reading
	}
	Log.verbose ("<========================================= End =========================================>" dendl);

	if (line.indexOf ("{\"status\"") == 1) // The first charactere is a nl
	{
		Log.trace ("Success !" dendl);

		line = line.substring (1); // The first charactere is a nl, so we don't want it
	}
	else
	{
		Log.error ("Failed ! (%d)" dendl, line.indexOf ("{\"status\""));
	}

	Log.trace ("Closing connection" dendl);

	// At this point, the json of the answer is in the string,
	// that's actually the line we want

	Log.verbose ("Json: \"%s\"" dendl, line.c_str());

	// Deserialize the JSON document
	deserializeJson (doc, line.c_str(), line.length());

	status    = ((const char *) doc["status"]);
	timestamp = doc["timestamp"];

	Log.trace ("Status : |%s| " dendl, status.c_str());

	if (status.indexOf ("OK") != 0)
	{
		Log.error ("Wrong status, leaving. (%s)" dendl, status.c_str());
		return;
	}

	Log.trace ("Timestamp: %l" dendl, timestamp);

	setTime (timestamp);

	Log.trace ("Time set!" dendl);
} // Wifi::getTime

void Wifi::receiveAndDecode ()
{
	clientListItem &item = list.getItem (server);
	WiFiClient &client   = item.client;

	if (!client.connected())
	{
		list.removeItem (item);
		return;
	}

	switch (item.state)
	{
		case connected:
			Log.trace ("Received request from %s" dendl, client.remoteIP().toString().c_str());

			item.timeout = millis();
			item.count   = millis();
			item.state   = awaitingData;

			break;

		case awaitingData:
			if (client.available())
			{
				item.state = processData;
				break;
			}

			if (millis() - clientTimeout > CLIENT_TIMEOUT)
			{
				Log.warning ("Client data timed out" dendl);

				list.removeItem (item);
				Log.trace ("Removing item - n = %d" dendl, list.getLength());
			}
			break;

		case processData:
			String str;

			str = client.readStringUntil ('\r');
			client.flush();

			Log.trace ("Request: |%s|" dendl, str.c_str());

			if (!str.startsWith ("GET"))
			{
				Log.warning ("Not a GET request, ignoring" dendl);
				utils.printHeader (client);
				client.print ("Use GET instead");
			}
			else if (str.startsWith ("GET /favicon.ico"))
			{
				Log.trace ("Favicon request, ignoring..." dendl);
				utils.printHeader (client);
				client.print ("No favicon");
			}
			else
			{
				str = str.substring (5, str.indexOf (' ', 5)); // Trimming the string from the end of "GET /" to where we meet the first space

				Req requestData = request.decode (str);

				if (requestData.type == requestInfos)
				{
					Log.trace ("Sending to arduino: Nothing" dendl);
					json.send ((char *) "OK", (char *) "", &client);
				}
				else if (requestData.error != none)
				{
					Log.trace ("Sending to arduino: Nothing" dendl);
					json.send ((char *) "ERROR", (char *) utils.errorTypeName (requestData.error, true), &client);
				}
				else
				{
					String data = utils.messageTypeName (requestData.type, true);
					if (utils.messageTypeComplementType (requestData.type) != COMPLEMENT_TYPE_NONE) data += requestData.complement;
					data += utils.ltos (requestData.information, requestData.type == RGB ? HEX : DEC);
					data += 'z';

					Log.trace ("Sending to arduino: %s" dendl, data.c_str());
					Serial1.print (data);

					json.send ((char *) "OK", (char *) "", &client);
				}
			}

			list.removeItem (item);
			Log.trace ("Removing item - n = %d" dendl, list.getLength());

			break;
	}
	
	delay(10);
} // Wifi::receiveAndDecode

Wifi wifi = Wifi();

#endif // if defined(LUMOS_ESP8266)