#include <Arduino.h>

#if defined(LUMOS_ESP8266)

# include <ArduinoJson.h>
# include <TimeLib.h>
# include "Utils.h"
# include "ArduinoLogger.h"
# include "Wifi.h"
# include "Request.h"
# include "Json.h"
# include "SerialCom.h"

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

	trace << "Connecting to " SSID0;

	WiFi.begin (SSID0, PASS0);

	time_t a = millis();
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

	server.on ("/", _handleRoot);
	server.on ("/command", _handleCommand);
	server.on ("/getRes", _handleGetRes);
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
{
	server.handleClient();
}

void Wifi::handleRoot ()
{
	trace << "Received request from " << server.client().localIP().toString() << endl;

	trace << "Request: |/|";

	server.sendHeader ("Location", "/index.html", true);
	server.send (302, "text/html", "");
}

void Wifi::handleCommand ()
{
	String message;

	trace << "Received request from " << server.client().localIP().toString() << endl;

	trace << "Request: |" << server.uri() << "|";
	for (int i = 0; i < server.args(); i++)
		trace << " |" << server.argName (i) << "=" << server.arg (i) << "|";
	trace << endl;

	Request request (server.arg ("type"), server.arg ("comp"), server.arg ("value"));

	request.process();

	if (request.getError() != RequestError::none)
	{
		trace << "Sending to arduino: Nothing" << dendl;
		message = json.getDataPretty ("ERROR", request.getError().toString().c_str());
	}
	else
	{
		if (request.getType() == RequestType::requestInfos)
			trace << "Sending to arduino: Nothing" << dendl;
		else
		{
			String data = request.getType().toString (true) + request.getComplementString() + request.getInformationString() + 'z';

			trace << "Sending to arduino: " << data << endl;

			serial.comSerialTx.print (data);
		}

		message = json.getDataPretty ("OK", "");
	}

	server.send (200, "application/json", message);
} // Wifi::handleCommand

void Wifi::handleGetRes ()
{
	String message;

	trace << "Received request from " << server.client().localIP().toString() << endl;

	trace << "Request: |" << server.uri() << "|";
	for (int i = 0; i < server.args(); i++)
		trace << " |" << server.argName (i) << "=" << server.arg (i) << "|";
	trace << dendl;

	message = json.getResourcesPretty();

	server.send (200, "application/json", message);
} // Wifi::handleGetStrings

void Wifi::handleWebRequests ()
{
	trace << "Received request from " << server.client().localIP().toString() << endl;

	trace << "Request: |" << server.uri() << "|";
	for (int i = 0; i < server.args(); i++)
		trace << " |" << server.argName (i) << "=" << server.arg (i) << "|";
	trace << dendl;

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
		verb << "File exists" << endl;

		server.streamFile (dataFile, dataType);

		verb << "File sent" << dendl;

		fileFound = true;
	}


	dataFile.close();
	return fileFound;
} // Wifi::loadFromSpiffs

Wifi wifi = Wifi();

#endif // if defined(LUMOS_ESP8266)