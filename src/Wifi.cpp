#include <Arduino.h>

#if defined(LUMOS_ESP8266)

# include <ArduinoJson.h>
# include <TimeLib.h>
# include "Utils.h"
# include "Logger.h"
# include "Wifi.h"
# include "Request.h"
# include "Json.h"
# include "ESPSerial.h"

void _handleRoot (){ wifi.handleRoot(); }

void _handleCommand (){ wifi.handleCommand(); }

void _handleWebRequests (){ wifi.handleWebRequests(); }

void _handleGetRes (){ wifi.handleGetRes(); }

Wifi::Wifi() : server (80)
{ }

Wifi::Wifi (Wifi && copy)
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

	SPIFFS.begin();

	server.on ("/", _handleRoot);
	server.on ("/command", _handleCommand);
	server.on ("/getRes", _handleGetRes);
	server.onNotFound (_handleWebRequests);
	server.begin();

	Log.trace ("Server started" endl);
	Log.trace ("Local IP: %s" dendl, WiFi.localIP().toString().c_str());
} // Wifi::init

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

	Log.trace ("Connecting to %s" endl, TIME_HOST);

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
		Log.trace ("Success !" endl);

		line = line.substring (1); // The first charactere is a nl, so we don't want it
	}
	else
	{
		Log.error ("Failed ! (%d)" endl, line.indexOf ("{\"status\""));
	}

	Log.trace ("Closing connection" endl);

	// At this point, the json of the answer is in the string,
	// that's actually the line we want

	Log.verbose ("Json: \"%s\"" endl, line.c_str());

	// Deserialize the JSON document
	deserializeJson (doc, line.c_str(), line.length());

	status    = ((const char *) doc["status"]);
	timestamp = doc["timestamp"];

	Log.trace ("Status : |%s| " endl, status.c_str());

	if (status.indexOf ("OK") != 0)
	{
		Log.error ("Wrong status, leaving. (%s)" endl, status.c_str());
		return;
	}

	Log.trace ("Timestamp: %l" endl, timestamp);

	setTime (timestamp);

	Log.trace ("Time set!" dendl);
} // Wifi::getTime

void Wifi::receiveAndDecode ()
{
	server.handleClient();
}

void Wifi::handleRoot ()
{
	Log.trace ("Received request from %s" endl, server.client().localIP().toString().c_str());

	Log.trace ("Request: |/|");

	server.sendHeader ("Location", "/index.html", true);
	server.send (302, "text/html", "");
}

void Wifi::handleCommand ()
{
	RequestData requestData;
	String message;

	Log.trace ("Received request from %s" endl, server.client().localIP().toString().c_str());

	Log.trace ("Request: |%s|", server.uri().c_str());
	for (int i = 0; i < server.args(); i++)
		Log.tracenp (" |%s=%s|", server.argName (i).c_str(), server.arg (i).c_str());
	Log.tracenp (dendl);

	requestData = request.decode (server.arg ("type"), server.arg ("comp"), server.arg ("value"));

	if (requestData.error != noError)
	{
		Log.trace ("Sending to arduino: Nothing" dendl);
		message = json.getDataPretty ("ERROR", utils.getErrorName (requestData.error));
	}
	else
	{
		if (requestData.type == requestInfos)
			Log.trace ("Sending to arduino: Nothing" dendl);
		else
		{
			String data = utils.getMessageTypeName (requestData.type);
			if (utils.messageTypeComplementType (requestData.type) != COMPLEMENT_TYPE_NONE) data += requestData.complement;
			data += utils.ltos (requestData.information, requestData.type == RGB ? HEX : DEC);
			data += 'z';

			Log.trace ("Sending to arduino: %s" dendl, data.c_str());
			serial.comSerialTx.print (data);
		}

		message = json.getDataPretty ("OK", "");
	}

	server.send (200, "application/json", message);
} // Wifi::handleCommand

void Wifi::handleGetRes ()
{
	RequestData requestData;
	String message;

	Log.trace ("Received request from %s" endl, server.client().localIP().toString().c_str());

	Log.trace ("Request: |%s|", server.uri().c_str());
	for (int i = 0; i < server.args(); i++)
		Log.tracenp (" |%s=%s|", server.argName (i).c_str(), server.arg (i).c_str());
	Log.tracenp (dendl);

	message = json.getResourcesPretty();

	server.send (200, "application/json", message);
} // Wifi::handleGetStrings

void Wifi::handleWebRequests ()
{
	Log.trace ("Received request from %s" endl, server.client().localIP().toString().c_str());

	Log.trace ("Request: |%s|", server.uri().c_str());
	for (int i = 0; i < server.args(); i++)
		Log.tracenp (" |%s=%s|", server.argName (i).c_str(), server.arg (i).c_str());
	Log.tracenp (dendl);

	if (loadFromSpiffs (server.uri()))
		return;

	String message = "File Not Detected\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++)
	{
		message += " NAME:" + server.argName (i) + "\n VALUE:" + server.arg (i) + "\n";
	}
	server.send (404, "text/plain", message);
}

bool Wifi::loadFromSpiffs (String path)
{
	String dataType = "text/plain";
	bool fileFound  = false;

	if (path.endsWith ("/")) path += "index.html";

	if (server.hasArg ("download")) dataType = "application/octet-stream";

	if (path.endsWith (".src")) path = path.substring (0, path.lastIndexOf ("."));
	else if (path.endsWith (".html")) dataType = "text/html";
	else if (path.endsWith (".htm")) dataType = "text/html";
	else if (path.endsWith (".css")) dataType = "text/css";
	else if (path.endsWith (".js")) dataType = "application/javascript";
	else if (path.endsWith (".png")) dataType = "image/png";
	else if (path.endsWith (".gif")) dataType = "image/gif";
	else if (path.endsWith (".jpg")) dataType = "image/jpeg";
	else if (path.endsWith (".ico")) dataType = "image/x-icon";
	else if (path.endsWith (".xml")) dataType = "text/xml";
	else if (path.endsWith (".pdf")) dataType = "application/pdf";
	else if (path.endsWith (".zip")) dataType = "application/zip";

	File dataFile = SPIFFS.open (path.c_str(), "r");

	if (dataFile.isFile())
	{
		Log.verbose ("File exists" endl);

		server.streamFile (dataFile, dataType);

		Log.verbose ("File sent" dendl);

		fileFound = true;
	}


	dataFile.close();
	return fileFound;
} // Wifi::loadFromSpiffs

Wifi wifi = Wifi();

#endif // if defined(LUMOS_ESP8266)