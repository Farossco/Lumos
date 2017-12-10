#ifndef LOGGING_H
#define LOGGING_H

#include <inttypes.h>
#include <stdarg.h>
#include <Arduino.h>
#include <Time.h>

#define LEVEL_SILENT  0
#define LEVEL_ERROR   1
#define LEVEL_WARNING 2
#define LEVEL_INFO    3
#define LEVEL_TRACE   4
#define LEVEL_VERBOSE 5

const boolean SERIAL_LOG_ENABLED = true;        // Serial logging
const int SERIAL_LEVEL           = LEVEL_TRACE; // Serial log level
const boolean SD_LOG_ENABLED     = true;        // SD logging
const int SD_LEVEL               = LEVEL_INFO;  // SD logging level

class Logging
{
private:
	int serialLevel, sdLevel;
	bool multiOutput;
	HardwareSerial * serial;
	Print * file;
	char buf[20];

public:
	Logging();

	void begin (HardwareSerial * serial, long serialBaudRate, int serialLevel);
	void begin (HardwareSerial * serial, long serialBaudRate, int serialLevel, Print * file, int sdLevel);

	template <class T> void error (T msg, ...)
	{
		if (serialLevel >= LEVEL_ERROR)
		{
			serial->print (clock (buf));
			serial->print (" [  ERROR  ] ");

			va_list args;
			va_start (args, msg);
			print (serial, msg, args);
		}
	}

	template <class T> void errornp (T msg, ...)
	{
		if (serialLevel >= LEVEL_ERROR)
		{
			va_list args;
			va_start (args, msg);
			print (serial, msg, args);
		}
	}

	template <class T> void warning (T msg, ...)
	{
		if (serialLevel >= LEVEL_WARNING)
		{
			serial->print (clock (buf));
			serial->print (" [ WARNING ] ");

			va_list args;
			va_start (args, msg);
			print (serial, msg, args);
		}
	}

	template <class T> void warningnp (T msg, ...)
	{
		if (serialLevel >= LEVEL_WARNING)
		{
			serial->print (clock (buf));
			serial->print (" [ WARNING ] ");

			va_list args;
			va_start (args, msg);
			print (serial, msg, args);
		}
	}

	template <class T> void info (T msg, ...)
	{
		if (serialLevel >= LEVEL_INFO)
		{
			serial->print (clock (buf));
			serial->print (" [  INFO   ] ");

			va_list args;
			va_start (args, msg);
			print (serial, msg, args);
		}
	}

	template <class T> void infonp (T msg, ...)
	{
		if (serialLevel >= LEVEL_INFO)
		{
			va_list args;
			va_start (args, msg);
			print (serial, msg, args);
		}
	}

	template <class T> void trace (T msg, ...)
	{
		serial->print (clock (buf));
		serial->print (" [  TRACE  ] ");

		va_list args;
		va_start (args, msg);
		print (serial, msg, args);
	}

	template <class T> void tracenp (T msg, ...)
	{
		va_list args;

		va_start (args, msg);
		print (serial, msg, args);
	}

	template <class T> void verbose (T msg, ...)
	{
		serial->print (clock (buf));
		serial->print (" [ VERBOSE ] ");

		va_list args;
		va_start (args, msg);
		print (serial, msg, args);
	}

	template <class T> void verbosenp (T msg, ...)
	{
		va_list args;

		va_start (args, msg);
		print (serial, msg, args);
	}

private:
	void print (Print * output, const char * format, va_list args);
	void print (Print * output, const __FlashStringHelper * format, va_list args);
	void printFormat (Print * output, const char format, va_list * args);
	char * clock (char * buf);
};

extern Logging Log;
#endif // ifndef LOGGING_H
