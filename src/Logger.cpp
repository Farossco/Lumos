#include "Logger.h"

Print * Logger::output1         = NULL;
Print * Logger::output2         = NULL;
uint8_t Logger::output1Level    = LEVEL_SILENT;
uint8_t Logger::output2Level    = LEVEL_SILENT;
bool Logger::output1Initialized = false;
bool Logger::output2Initialized = false;

Logger::Logger (uint8_t level) : level (level)
{
	prefixPrint = true;
}

void Logger::init (Print & initOutput1, uint8_t initOutput1Level, Print & initOutput2, uint8_t initOutput2Level)
{
	output1Level       = constrain (initOutput1Level, LEVEL_SILENT, LEVEL_VERBOSE);
	output1            = &initOutput1;
	output1Initialized = true;

	if (&initOutput2 != NULL && initOutput2Level != LEVEL_SILENT)
	{
		output2Level       = constrain (initOutput2Level, LEVEL_SILENT, LEVEL_VERBOSE);
		output2            = &initOutput2;
		output2Initialized = true;
	}

	setflags();

	inf << "Log initialized." << dendl;
}

void Logger::setPrefixPrint (bool prefixprint)
{
	this->prefixPrint = prefixprint;
}

bool Logger::isEnabledFor (int level, int output)
{
	return output == 1 ? (output1Level >= level) : output == 2 ? (output2Level >= level) : 0;
}

void Logger::setflags ()
{
	flags (dec | right | skipws | showbase | uppercase | boolalpha);
}

/** Output string
 * \param[in] arg string to output
 * \return the stream
 */
Logger &operator << (ostream & s, Logger& (*pf)(Logger & logger))
{
	return pf ((Logger&) s);
}

void Logger::putch (char c)
{
	if (output1Initialized && output1Level >= level)
	{
		printPrefix (output1);
		output1->write (c);
	}
	if (output2Initialized && output2Level >= level)
	{
		printPrefix (output2);
		output2->write (c);
	}
}

void Logger::putstr (const char * str)
{
	if (output1Initialized && output1Level >= level)
	{
		printPrefix (output1);
		output1->write (str);
	}
	if (output2Initialized && output2Level >= level)
	{
		printPrefix (output2);
		output2->write (str);
	}
}

bool Logger::seekoff (off_type off, seekdir way)
{
	(void) off;
	(void) way;
	return false;
}

bool Logger::seekpos (pos_type pos)
{
	(void) pos;
	return false;
}

bool Logger::Logger::sync ()
{
	return true;
}

Logger::pos_type Logger::tellpos ()
{
	return 0;
}

void Logger::printPrefix (Print * output)
{
	if (prefixPrint)
	{
		output->print (F ("["));
		output->print (utils.clock());
		output->print (F ("] ["));

		output->print (output1Initialized + output2Initialized);
		output->print (F ("] ["));

		output->print (debugLevelName (level));
		output->print (debugLevelSpace (level));
		output->print (F ("] "));

		prefixPrint = false;
	}
}

const char * Logger::debugLevelName (uint8_t debugLevel)
{
	switch (debugLevel)
	{
		case LEVEL_ERROR:
			return "ERROR";

		case LEVEL_WARNING:
			return "WARNING";

		case LEVEL_INFO:
			return "INFO";

		case LEVEL_TRACE:
			return "TRACE";

		case LEVEL_VERBOSE:
			return "VERBOSE";

		default:
			return "???????";
	}
}

const char * Logger::debugLevelSpace (uint8_t debugLevel)
{
	switch (debugLevel)
	{
		case LEVEL_INFO:
			return "   ";

		case LEVEL_ERROR:
		case LEVEL_TRACE:
			return "  ";

		case LEVEL_WARNING:
		case LEVEL_VERBOSE:
		default:
			return "";
	}
}

Logger err   (LEVEL_ERROR);
Logger warn  (LEVEL_WARNING);
Logger inf   (LEVEL_INFO);
Logger trace (LEVEL_TRACE);
Logger verb  (LEVEL_VERBOSE);
Logger logger;