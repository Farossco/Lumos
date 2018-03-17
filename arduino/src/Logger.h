#ifndef LOGGING_H
#define LOGGING_H

#include <Arduino.h>
#include "Global.h"
#include "SdCard.h"

#define LEVEL_SILENT  0
#define LEVEL_ERROR   1
#define LEVEL_WARNING 2
#define LEVEL_INFO    3
#define LEVEL_TRACE   4
#define LEVEL_VERBOSE 5

#define endl          "\n"
#define dendl         "\n\n" // double endl

class Logger
{
private:
	int output1Level, output2Level;
	bool multiOutput;
	Print * output1, * output2;
	char buf[35];

public:
	Logger();

	void init (Print * output1, int output1Level);
	void init (Print * output1, int output1Level, Print * output2, int output2Level);

	template <class T, typename ... Args> void error (T msg, Args ... args)
	{
		printLevel (true, LEVEL_ERROR, msg, args ...);
	}

	template <class T, typename ... Args> void errornp (T msg, Args ... args)
	{
		printLevel (false, LEVEL_ERROR, msg, args ...);
	}

	template <class T, typename ... Args> void warning (T msg, Args ... args)
	{
		printLevel (true, LEVEL_WARNING, msg, args ...);
	}

	template <class T, typename ... Args> void warningnp (T msg, Args ... args)
	{
		printLevel (false, LEVEL_WARNING, msg, args ...);
	}

	template <class T, typename ... Args> void info (T msg, Args ... args)
	{
		printLevel (true, LEVEL_INFO, msg, args ...);
	}

	template <class T, typename ... Args> void infonp (T msg, Args ... args)
	{
		printLevel (false, LEVEL_INFO, msg, args ...);
	}

	template <class T, typename ... Args> void trace (T msg, Args ... args)
	{
		printLevel (true, LEVEL_TRACE, msg, args ...);
	}

	template <class T, typename ... Args> void tracenp (T msg, Args ... args)
	{
		printLevel (false, LEVEL_TRACE, msg, args ...);
	}

	template <class T, typename ... Args> void verbose (T msg, Args ... args)
	{
		printLevel (true, LEVEL_VERBOSE, msg, args ...);
	}

	template <class T, typename ... Args> void verbosenp (T msg, Args ... args)
	{
		printLevel (false, LEVEL_VERBOSE, msg, args ...);
	}

private:
	void print (Print * output, const char * format, va_list args);
	void print (Print * output, const __FlashStringHelper * format, va_list args);
	void printFormat (Print * output, const char format, va_list * args);
	template <class T> void printLevel (boolean showPrefix, int level, T msg, ...)
	{
		if (multiOutput && output2Level >= level && sd.isEnabled())
		{
			if (showPrefix)
			{
				sd.openFile();

				output2->print ("[");

				output2->print (utils.clock (buf));
				output2->print ("]");
				output2->print (" [ ");
				output2->print (debugLevelName (level));
				output2->print (debugLevelSpace (level));
				output2->print (" ] ");
			}

			va_list args;
			va_start (args, msg);
			print (output2, msg, args);
		}

		if (output1Level >= level)
		{
			if (showPrefix)
			{
				output1->print (F ("["));

				output1->print (utils.clock (buf));
				output1->print (F ("]"));
				output1->print (F (" [ "));
				output1->print (debugLevelName (level));
				output1->print (debugLevelSpace (level));
				output1->print (F (" ] "));
			}

			va_list args;
			va_start (args, msg);
			print (output1, msg, args);
		}
	} // printLevel

	const char * debugLevelName (int debugLevel);
	const char * debugLevelSpace (int debugLevel);
};

extern Logger Log;
#endif // ifndef LOGGING_H
