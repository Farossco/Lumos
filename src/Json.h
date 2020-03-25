#ifndef JSON_H
#define JSON_H

#include <Arduino.h>

#if defined(LUMOS_ESP8266)

# include <ESP8266WiFi.h>

class Json
{
public:
	Json();
	String getPretty (const char * status, const char * message);
	String get (const char * status, const char * message, bool pretty = false);
};

extern Json json;

#endif // if defined(LUMOS_ESP8266)

#endif // ifndef JSON_H