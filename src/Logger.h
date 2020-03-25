#ifndef LOGGING_H
#define LOGGING_H

#include <Arduino.h>
#include "Utils.h"

#if defined(LUMOS_ARDUINO_MEGA)
# include "SdCard.h"
#endif

#define LEVEL_SILENT  0
#define LEVEL_ERROR   1
#define LEVEL_WARNING 2
#define LEVEL_INFO    3
#define LEVEL_TRACE   4
#define LEVEL_VERBOSE 5

#define endl          "\n"
#define dendl         "\n\n" // double endl

const boolean SERIAL_LOG_ENABLED = true; // Serial logging
const boolean SD_LOG_ENABLED     = true; // SD logging

class Logger
{
private:
	uint8_t output1Level, output2Level;
	bool multiOutput;
	Print * output1, * output2;

public:
	Logger();

	void init (Print & output1, uint8_t output1Level);
	void init (Print & output1, uint8_t output1Level, Print & output2, uint8_t output2Level);
	bool isEnabledFor (int level, int output = 1);

	template <class T, typename ... Args> void error (T msg, Args ... args){ printLevel (true, LEVEL_ERROR, msg, args ...); }

	template <class T, typename ... Args> void errornp (T msg, Args ... args){ printLevel (false, LEVEL_ERROR, msg, args ...); }

	template <class T, typename ... Args> void warning (T msg, Args ... args){ printLevel (true, LEVEL_WARNING, msg, args ...); }

	template <class T, typename ... Args> void warningnp (T msg, Args ... args){ printLevel (false, LEVEL_WARNING, msg, args ...); }

	template <class T, typename ... Args> void info (T msg, Args ... args){ printLevel (true, LEVEL_INFO, msg, args ...); }

	template <class T, typename ... Args> void infonp (T msg, Args ... args){ printLevel (false, LEVEL_INFO, msg, args ...); }

	template <class T, typename ... Args> void trace (T msg, Args ... args){ printLevel (true, LEVEL_TRACE, msg, args ...); }

	template <class T, typename ... Args> void tracenp (T msg, Args ... args){ printLevel (false, LEVEL_TRACE, msg, args ...); }

	template <class T, typename ... Args> void verbose (T msg, Args ... args){ printLevel (true, LEVEL_VERBOSE, msg, args ...); }

	template <class T, typename ... Args> void verbosenp (T msg, Args ... args){ printLevel (false, LEVEL_VERBOSE, msg, args ...); }

private:
	void print (Print * output, const char * format, va_list args);
	void print (Print * output, const __FlashStringHelper * format, va_list args);
	void printFormat (Print * output, const char format, va_list * args);
	void printPrefix (Print * output, uint8_t level)
	{
		output->print (F ("["));
		output->print (utils.clock ());
		output->print (F ("] ["));

		#if defined(LUMOS_ARDUINO_MEGA)

		if (sd.isEnabled())
			output->print (F ("CARD] ["));
		else
			output->print (F ("NOCA] ["));

		#endif

		output->print (debugLevelName (level));
		output->print (debugLevelSpace (level));
		output->print (F ("] "));
	}

	template <class T> void printLevel (boolean showPrefix, uint8_t level, T msg, ...)
	{
		#if defined(LUMOS_ARDUINO_MEGA)

		if (multiOutput && output2Level >= level && sd.isEnabled())
		{
			sd.openFile();

			if (showPrefix)
				printPrefix (output2, level);

			va_list args;
			va_start (args, msg);
			print (output2, msg, args);
		}

		#endif // if defined(LUMOS_ARDUINO_MEGA)

		if (output1Level >= level)
		{
			if (showPrefix)
				printPrefix (output1, level);

			va_list args;
			va_start (args, msg);
			print (output1, msg, args);
		}
	}

	const char * debugLevelName (uint8_t debugLevel);
	const char * debugLevelSpace (uint8_t debugLevel);
};

extern Logger Log;
#endif // ifndef LOGGING_H