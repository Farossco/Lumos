#ifndef ESP_WIFI_H
#define ESP_WIFI_H

#include <Arduino.h>

#if defined(ESP8266_PERI_H_INCLUDED)

# include <ESP8266WiFi.h>

// Location
# define LATITUDE    "48.866667"    // My latitude (Well... not really)
# define LONGITUDE   "2.333333"     // My longitude (Not really either :p)
# define TIME_ZONE   "Europe/Paris" // My time zone

# define TIME_FORMAT "json"         // Format for receive the time
# define TIME_KEY    "0D2WZ3KAP6GV" // TimeZoneDB API key
# define TIME_BY     "zone"         // localization method
# define TIME_FIELDS "timestamp"    // Needed fields in the answer

// Wi-Fi informations
# define SSID0 "Wificlariopc" // Wi-Fi SSID
# define PASS0 "FC00000000"   // Wi-Fi password

// Time
const int TIME_REQUEST_TIMEOUT = 15000; // Request timeout
const int TIME_HTTP_PORT       = 80;    // HTPP port
# define TIME_HOST "api.timezonedb.com" // HTPP host

class Wifi
{
public:
	Wifi();
	void init ();
	void getTime ();
	void receiveAndDecode ();

	WiFiClient client;

private:
	WiFiServer server;
};

extern Wifi wifi;

#endif // if defined(ESP8266_PERI_H_INCLUDED)

#endif // ifndef ESP_WIFI_H
