#include "ArduinoLog.h"
#include <stdlib.h>

Logging::Logging()
{
	serialLevel = LEVEL_SILENT;
	sdLevel     = LEVEL_SILENT;
	multiOutput = false;
	serial      = NULL;
	file        = NULL;
}

void Logging::begin (HardwareSerial * serial, long serialBaudRate, int serialLevel)
{
	serial->begin (serialBaudRate);

	while (!Serial && !Serial.available())
		;

	this->serialLevel = constrain (serialLevel, LEVEL_SILENT, LEVEL_VERBOSE);
	this->serial      = serial;
}

void Logging::begin (HardwareSerial * serial, long serialBaudRate, int serialLevel, Print * file, int sdLevel)
{
	begin (serial, serialBaudRate, serialLevel);

	this->sdLevel = constrain (sdLevel, LEVEL_SILENT, LEVEL_VERBOSE);
	this->file    = file;

	multiOutput = true;
}

char * Logging::clock (char * buf)
{
	sprintf (buf, "%.2d/%.2d/%.4d %.2d:%.2d:%.2d", day(), month(), year(), hour(), minute(), second());

	return buf;
}

void Logging::print (Print * output, const __FlashStringHelper * format, va_list args)
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

void Logging::print (Print * output, const char * format, va_list args)
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

void Logging::printFormat (Print * output, const char format, va_list * args)
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
		output->print (va_arg (*args, int), HEX);
		return;
	}

	if (format == 'X')
	{
		output->print ("0x");
		output->print (va_arg (*args, int), HEX);
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
			output->print (F ("true"));
		}
		else
		{
			output->print (F ("false"));
		}
		return;
	}
} // Logging::printFormat

Logging Log = Logging();
