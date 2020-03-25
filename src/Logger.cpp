#include <stdlib.h>
#include <inttypes.h>
#include <stdarg.h>
#include <Arduino.h>
#include <Time.h>
#include "Logger.h"

Logger::Logger()
{
	output1Level = LEVEL_SILENT;
	output2Level = LEVEL_SILENT;
	multiOutput  = false;
	output1      = NULL;
	output2      = NULL;
}

void Logger::init (Print & output1, uint8_t output1Level)
{
	this->output1Level = constrain (output1Level, LEVEL_SILENT, LEVEL_VERBOSE);
	this->output1      = &output1;

	multiOutput = false;
}

void Logger::init (Print & output1, uint8_t output1Level, Print & output2, uint8_t output2Level)
{
	init (output1, output1Level);

	this->output2Level = constrain (output2Level, LEVEL_SILENT, LEVEL_VERBOSE);
	this->output2      = &output2;

	multiOutput = true;
}

bool Logger::isEnabledFor (int level, int output)
{
	return output == 1 ? (output1Level >= level) : output == 2 ? (output2Level >= level) : 0;
}

void Logger::print (Print * output, const __FlashStringHelper * format, va_list args)
{
	PGM_P p = reinterpret_cast<PGM_P>(format);
	char c  = pgm_read_byte (p++);

	for (; c != 0; c = pgm_read_byte (p++))
	{
		if (c == '%')
		{
			c = pgm_read_byte (p++);
			printFormat (output, c, &args);
		}
		else
		{
			output->print (c);
		}
	}
}

void Logger::print (Print * output, const char * format, va_list args)
{
	for (; *format != 0; ++format)
	{
		if (*format == '%')
		{
			++format;
			printFormat (output, *format, &args);
		}
		else
		{
			output->print (*format);
		}
	}
}

void Logger::printFormat (Print * output, const char format, va_list * args)
{
	if (format == '\0') return;

	if (format == '%')
	{
		output->print (format);
		return;
	}

	if (format == 's')
	{
		register char * s = (char *) va_arg (*args, int);
		output->print (s);
		return;
	}

	if (format == 'd' || format == 'i')
	{
		output->print (va_arg (*args, int), DEC);
		return;
	}

	if (format == 'D' || format == 'F')
	{
		output->print (va_arg (*args, double));
		return;
	}

	if (format == 'x')
	{
		output->print (va_arg (*args, long), HEX);
		return;
	}

	if (format == 'X')
	{
		output->print ("0x");
		output->print (va_arg (*args, long), HEX);
		return;
	}

	if (format == 'b')
	{
		output->print (va_arg (*args, int), BIN);
		return;
	}

	if (format == 'B')
	{
		output->print ("0b");
		output->print (va_arg (*args, int), BIN);
		return;
	}

	if (format == 'l')
	{
		output->print (va_arg (*args, long), DEC);
		return;
	}

	if (format == 'c')
	{
		output->print ((char) va_arg (*args, int));
		return;
	}

	if (format == 't')
	{
		if (va_arg (*args, int) == 1)
		{
			output->print ("T");
		}
		else
		{
			output->print ("F");
		}
		return;
	}

	if (format == 'T')
	{
		if (va_arg (*args, int) == 1)
		{
			output->print (F ("True"));
		}
		else
		{
			output->print (F ("False"));
		}
		return;
	}
} // Logging::printFormat

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

Logger Log = Logger();