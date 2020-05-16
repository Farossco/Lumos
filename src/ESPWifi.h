#ifndef WIFI_H
#define WIFI_H

#if defined(LUMOS_ESP32)

# include <WiFi.h>
# include <ESPAsyncWebServer.h>

# define TIME_HOST   "api.timezonedb.com" // HTPP time host
# define TIME_FORMAT "json"               // Format for receive the time
# define TIME_KEY    ESP_TIME_API_KEY     // TimeZoneDB API key
# define TIME_BY     "zone"               // localization method
# define TIME_FIELDS "timestamp"          // Needed fields in the answer

// Time
const int TIME_REQUEST_TIMEOUT = 15000; // Request timeout
const int TIME_HTTP_PORT       = 80;    // HTPP port

class Wifi
{
public:
	Wifi();

	void init ();
	void getTime ();
	void receiveAndDecode ();

	void displayRequest (AsyncWebServerRequest & rqst);
	void handleRoot (AsyncWebServerRequest & rqst);
	void handleCommand (AsyncWebServerRequest & rqst);
	void handleWebRequests (AsyncWebServerRequest & rqst);
	void handleGetRes (AsyncWebServerRequest & rqst);

private:
	AsyncWebServer server;
	bool loadFromSpiffs (String path, AsyncWebServerRequest & rqst);
};

extern Wifi wifi;

#endif // if defined(LUMOS_ESP32)

#endif // ifndef ESP_WIFI_H