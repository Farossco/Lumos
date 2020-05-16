#include <Arduino.h>

#if defined(LUMOS_ESP32)

# include <ArduinoJson.h>
# include <TimeLib.h>
# include "Utils.h"
# include "ArduinoLogger.h"
# include "ESPWifi.h"
# include "Request.h"
# include "Json.h"
# include "SerialCom.h"
# include "SPIFFS.h"

void _handleRoot (AsyncWebServerRequest * rqst){ wifi.handleRoot (*rqst); }

void _handleCommand (AsyncWebServerRequest * rqst){ wifi.handleCommand (*rqst); }

void _handleWebRequests (AsyncWebServerRequest * rqst){ wifi.handleWebRequests (*rqst); }

void _handleGetRes (AsyncWebServerRequest * rqst){ wifi.handleGetRes (*rqst); }

Wifi::Wifi() : server (80)
{ }

void Wifi::init ()
{
	WiFi.mode (WIFI_STA);

	trace << "Connecting to " SSID0;

	WiFi.begin (SSID0, PASS0);

	uint32_t a = millis();
	while (WiFi.status() != WL_CONNECTED)
	{
		if (millis() - a >= 200)
		{
			trace << ".";
			a = millis();
		}
		delay (1);
	}
	trace << endl;

	trace << "WiFi connected" << dendl;

	SPIFFS.begin();

	server.on ("/", HTTP_ANY, _handleRoot);
	server.on ("/command", HTTP_ANY, _handleCommand);
	server.on ("/getRes", HTTP_ANY, _handleGetRes);
	server.onNotFound (_handleWebRequests);
	server.begin();

	trace << "Server started" << endl;
	trace << "Local IP: " << WiFi.localIP().toString() << endl;
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

	trace << "Connecting to " TIME_HOST << endl;

	// Use WiFiClient class to create TCP connections
	if (!client.connect (TIME_HOST, TIME_HTTP_PORT))
	{
		err << "Connection failed" << dendl;
		return;
	}

	// We now create an url for the request

	// [DEBUG] Printing the url
	trace << "Requesting URL: " << url << endl;

	// Sending web request
	client.print (String ("GET ") + url + " HTTP/1.1\r\n" + "Host: " + TIME_HOST + "\r\n" + "Connection: close\r\n\r\n");
	timeout = millis();

	// Waiting for an answer
	while (!client.available())
	{
		// If nothing comes out after the timeout, we abort
		if (millis() - timeout > TIME_REQUEST_TIMEOUT)
		{
			err << "Client Timeout !" << dendl;
			client.stop();
			return;
		}
	}

	verb << "Server answer:" << endl;
	verb << "<================================ Start ================================>" << endl;

	// Read all the lines of the reply from server and print them to Serial
	while (client.available() && line.indexOf ("{\"status\"") == -1)
	{
		line = client.readStringUntil ('\r');
		verb << (line.charAt (0) == '\n' ? line.c_str() + 1 : line.c_str()) << endl; // [DEBUG] We print the line we're currently reading
	}
	verb << "<================================ End ==================================>" << endl;

	if (line.indexOf ("{\"status\"") == 1) // The first charactere is a nl
	{
		trace << "Success !" << endl;

		line = line.substring (1); // The first charactere is a nl, so we don't want it
	}
	else
	{
		err << "Failed ! (" << line.indexOf ("{\"status\"") << ")" << endl;
	}

	trace << "Closing connection" << endl;

	// At this point, the json of the answer is in the string,
	// that's actually the line we want

	verb << "Json: \"" << line << "\"" << endl;

	// Deserialize the JSON document
	deserializeJson (doc, line);

	status    = ((const char *) doc["status"]);
	timestamp = doc["timestamp"];

	trace << "Status : |" << status << "| " << endl;

	if (status.indexOf ("OK") != 0)
	{
		err << "Wrong status, leaving. (" << status << ")" << endl;
		return;
	}

	trace << "Timestamp: " << timestamp << endl;

	setTime (timestamp);

	trace << "Time set!" << dendl;
} // Wifi::getTime

void Wifi::receiveAndDecode ()
{ }

void Wifi::displayRequest (AsyncWebServerRequest & rqst)
{
	trace << "Received request from " << rqst.client()->remoteIP().toString() << endl;

	trace << "Request: |" << rqst.url() << "|";
	for (int i = 0; i < rqst.args(); i++)
		trace << " |" << rqst.argName (i) << "=" << rqst.arg (i) << "|";
	trace << endl;
}

void Wifi::handleRoot (AsyncWebServerRequest & rqst)
{
	displayRequest (rqst);

	rqst.send (SPIFFS, "/index.html", String(), false);
}

void Wifi::handleCommand (AsyncWebServerRequest & rqst)
{
	String message;

	displayRequest (rqst);

	Request request (rqst.arg ("type"), rqst.arg ("comp"), rqst.arg ("value"));

	request.process();

	if (request.getError())
	{
		trace << "Sending to arduino: Nothing" << dendl;
		message = json.getErrorPretty (request.getError());
	}
	else
	{
		if (request.getType() == RequestType::requestData)
			trace << "Sending to arduino: Nothing" << dendl;
		else
		{
			String data = request.getType().toString (true) + request.getComplementString() + request.getInformationString() + 'z';

			trace << "Sending to arduino: " << data << endl;

			serial.comSerial.print (data);
		}

		message = json.getDataPretty();
	}

	rqst.send (200, "application/json", message);
}

void Wifi::handleGetRes (AsyncWebServerRequest & rqst)
{
	displayRequest (rqst);

	rqst.send (200, "application/json", json.getResourcesPretty());
}

void Wifi::handleWebRequests (AsyncWebServerRequest & rqst)
{
	displayRequest (rqst);

	if (loadFromSpiffs (rqst.url(), rqst))
		return;

	String message = "File Not Detected\n\n";
	message += String ("URL: ") + rqst.url() + "\n";
	message += String ("Method: ") + ((rqst.method() == HTTP_GET) ? "GET" : "POST") + "\n";
	message += String ("Arguments: ") + rqst.args() + "\n";
	for (uint8_t i = 0; i < rqst.args(); i++)
		message += " NAME:" + rqst.argName (i) + "\n VALUE:" + rqst.arg (i) + "\n";

	rqst.send (404, "text/plain", message);
}

bool Wifi::loadFromSpiffs (String path, AsyncWebServerRequest & rqst)
{
	String dataType = "text/plain";
	bool fileFound  = false;

	if (path.endsWith ("/")) path += "index.html";

	if (rqst.hasArg ("download")) dataType = "application/octet-stream";

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

	if (SPIFFS.exists (path))
		rqst.send (SPIFFS, rqst.url(), dataType);

	return fileFound;
} // Wifi::loadFromSpiffs

Wifi wifi = Wifi();

#endif // if defined(LUMOS_ESP32)