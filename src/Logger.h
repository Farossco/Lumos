#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "ostream.h"
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

class Logger : public ostream
{
public:
	Logger(uint8_t level = LEVEL_SILENT);

	void init (Print & initOutput1, uint8_t initOutput1Level, Print & initOutput2 = * ((Print *) NULL), uint8_t initOutput2Level = LEVEL_SILENT);
	void setPrefixPrint (bool prefixPrint);
	bool isEnabledFor (int level, int output = 1);
	void setflags ()
	{
		flags (dec | right | skipws | showbase | uppercase | boolalpha);
	}

private:
	void putch (char c);
	void putstr (const char * str);
	bool seekoff (off_type off, seekdir way);
	bool seekpos (pos_type pos);
	bool sync ();
	pos_type tellpos ();

	const char * debugLevelName (uint8_t debugLevel);
	const char * debugLevelSpace (uint8_t debugLevel);
	void printPrefix (Print * output);

	const uint8_t level;
	bool prefixPrint;

	static Print * output1, * output2;
	static uint8_t output1Level, output2Level;
	static bool output1Initialized, output2Initialized;
};

/*
 * End of line
 */
inline ostream& endl (ostream& logger)
{
	logger.put ('\n');
	((Logger&) logger).setPrefixPrint (true);
	((Logger&) logger).setflags();

	return logger;
}

/*
 * Double end of line
 */
inline ostream& dendl (ostream& logger)
{
	logger.put ('\n');
	logger.put ('\n');
	((Logger&) logger).setPrefixPrint (true);
	((Logger&) logger).setflags();

	return logger;
}

/*
 * Do not display prefix for the current line
 * Must be put in second position
 */
inline ostream& np (ostream& logger)
{
	((Logger&) logger).setPrefixPrint (false);

	return logger;
}

extern Logger err;   // Error level logging
extern Logger warn;  // Warning level logging
extern Logger inf;   // Info level logging
extern Logger trace; // Trace level logging
extern Logger verb;  // Verbose level logging
extern Logger logger;

#endif // ifndef LOGGER_H