#include "wifi_com.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <HTTPClient.h>
#include "Utils.h"
#include "ArduinoLogger.h"
#include "Request.h"
#include "Json.h"
#include "serial_com.h"

#define TIME_HOST   "api.timezonedb.com" /* HTPP time host */
#define TIME_FORMAT "json"               /* Format for receive the time */
#define TIME_KEY    ESP_TIME_API_KEY     /* TimeZoneDB API key */
#define TIME_BY     "zone"               /* localization method */
#define TIME_FIELDS "timestamp"          /* Needed fields in the answer */

static AsyncWebServer server(80);

static bool load_from_spiffs(String path, AsyncWebServerRequest * rqst)
{
	String dataType = "text/plain";
	bool fileFound  = false;

	if (path.endsWith("/")) {
		path += "index.html";
	}

	if (rqst->hasArg("download")) {
		dataType = "application/octet-stream";
	}

	if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
	else if (path.endsWith(".html")) dataType = "text/html";
	else if (path.endsWith(".htm")) dataType = "text/html";
	else if (path.endsWith(".css")) dataType = "text/css";
	else if (path.endsWith(".js")) dataType = "application/javascript";
	else if (path.endsWith(".png")) dataType = "image/png";
	else if (path.endsWith(".gif")) dataType = "image/gif";
	else if (path.endsWith(".jpg")) dataType = "image/jpeg";
	else if (path.endsWith(".ico")) dataType = "image/x-icon";
	else if (path.endsWith(".xml")) dataType = "text/xml";
	else if (path.endsWith(".pdf")) dataType = "application/pdf";
	else if (path.endsWith(".zip")) dataType = "application/zip";

	if (SPIFFS.exists(path)) {
		rqst->send(SPIFFS, rqst->url(), dataType);
		fileFound = true;
	}

	return fileFound;
}

static void request_display(AsyncWebServerRequest * rqst)
{
	trace << "Received request from " << rqst->client()->remoteIP().toString() << endl;

	trace << "Request: |" << rqst->url() << "|";
	for (int i = 0; i < rqst->args(); i++) {
		trace << " |" << rqst->argName(i) << "=" << rqst->arg(i) << "|";
	}

	trace << endl;
}

static void handle_root(AsyncWebServerRequest * rqst)
{
	request_display(rqst);

	rqst->send(SPIFFS, "/index.html");
}

static void handle_command(AsyncWebServerRequest * rqst)
{
	String message;

	request_display(rqst);

	Request request(rqst->arg("type"), rqst->arg("comp"), rqst->arg("value"));

	request.process();

	if (request.getError()) {
		message = json.getErrorPretty(request.getError());
	} else {
		if (request.getType() != RequestType::requestData) {
			String data = request.getType().toString(true)
			  + request.getComplementString()
			  + request.getInformationString() + 'z';
		}

		message = json.getDataPretty();
	}

	rqst->send(200, "application/json", message);
}

static void handle_get_res(AsyncWebServerRequest * rqst)
{
	request_display(rqst);

	rqst->send(200, "application/json", json.getResourcesPretty());
}

static void handle_web_requests(AsyncWebServerRequest * rqst)
{
	request_display(rqst);

	if (load_from_spiffs(rqst->url(), rqst)) {
		return;
	}

	String message = "File Not Detected\n\n";
	message += String("URL: ") + rqst->url() + "\n";
	message += String("Method: ") + ((rqst->method() == HTTP_GET) ? "GET" : "POST") + "\n";
	message += String("Arguments: ") + rqst->args() + "\n";
	for (uint8_t i = 0; i < rqst->args(); i++)
		message += " NAME:" + rqst->argName(i) + "\n VALUE:" + rqst->arg(i) + "\n";

	rqst->send(404, "text/plain", message);
}

void wifi_com_init()
{
	uint32_t a;

	WiFi.mode(WIFI_STA);

	trace << "Connecting to " SSID0;

	WiFi.begin(SSID0, PASS0);

	a = millis();

	while (WiFi.status() != WL_CONNECTED) {
		if (millis() - a >= 200) {
			trace << ".";
			a = millis();
		}

		delay(1);
	}

	trace << endl;

	trace << "WiFi connected" << dendl;

	SPIFFS.begin();

	server.on("/",        HTTP_ANY, handle_root);
	server.on("/command", HTTP_ANY, handle_command);
	server.on("/getRes",  HTTP_ANY, handle_get_res);
	server.onNotFound(handle_web_requests);
	server.begin();

	trace << "Server started" << endl;
	trace << "Local IP: " << WiFi.localIP().toString() << endl;
}

void wifi_com_time_get()
{
	const size_t capacity = JSON_OBJECT_SIZE(3) /* root (status, message, timestamp) */
	  + 50;                                     /* String duplications + security margin */

	DynamicJsonDocument doc(capacity);

	HTTPClient http;
	String json, status;
	const char url[] = "http://" TIME_HOST "/v2.1/get-time-zone?format=" TIME_FORMAT "&key=" TIME_KEY "&by=" TIME_BY "&zone=" TIME_ZONE "&fields=" TIME_FIELDS;
	unsigned long timeout, timestamp;

	trace << "Connecting to " << url << endl;

	http.begin(url);

	int code = http.GET();

	trace << "HTTP response code: " << code << endl;

	if (code != HTTP_CODE_OK) {
		err << "Wrong HTTP response code, stopping here" << endl;
		http.end();
		return;
	}

	json = http.getString();
	trace << "HTTP body: " << json << dendl;

	trace << "Closing connection" << endl;
	http.end();

	/* Deserialize the JSON document */
	deserializeJson(doc, json);

	status    = ((const char *)doc["status"]);
	timestamp = doc["timestamp"];

	trace << "Status : |" << status << "| " << endl;

	if (status.indexOf("OK") != 0) {
		err << "Wrong status, leaving. (" << status << ")" << endl;
		return;
	}

	trace << "Timestamp: " << timestamp << endl;

	setTime(timestamp);

	trace << "Time set!" << dendl;
}
