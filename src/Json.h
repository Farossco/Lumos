#ifndef JSON_H
#define JSON_H

#include <Arduino.h>

#if defined(LUMOS_ESP8266)

# include <ESP8266WiFi.h>

class Json
{
public:
	Json();
	String getDataPretty (const char * status, const char * message);
	String getData (const char * status, const char * message, bool pretty = false);
	String getResourcesPretty ();
	String getResources (bool pretty = false);
};

extern Json json;

#endif // if defined(LUMOS_ESP8266)

#endif // ifndef JSON_H