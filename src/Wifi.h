#ifndef ESP_WIFI_H
#define ESP_WIFI_H

// #include <Arduino.h>

#if defined(LUMOS_ESP8266)

# include <ESP8266WebServer.h>

# define TIME_FORMAT "json"           // Format for receive the time
# define TIME_KEY    ESP_TIME_API_KEY // TimeZoneDB API key
# define TIME_BY     "zone"           // localization method
# define TIME_FIELDS "timestamp"      // Needed fields in the answer

// Time
const int TIME_REQUEST_TIMEOUT = 15000; // Request timeout
const int TIME_HTTP_PORT       = 80;    // HTPP port
const int CLIENT_TIMEOUT       = 5000;  // Client timeout in ms
# define TIME_HOST "api.timezonedb.com" // HTPP host

class Wifi
{
public:
	Wifi();
	Wifi(Wifi && copy);

	void init ();
	void getTime ();
	void receiveAndDecode ();

	void handleRoot ();
	void handleCommand ();
	void handleWebRequests ();
	void handleGetRes ();

private:
	ESP8266WebServer server;
	bool loadFromSpiffs (String path);
};

extern Wifi wifi;

#endif // if defined(LUMOS_ESP8266)

#endif // ifndef ESP_WIFI_H