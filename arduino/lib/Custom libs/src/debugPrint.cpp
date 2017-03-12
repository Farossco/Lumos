#include <Arduino.h>

#define DEBUG_ENABLED true // Debug Mode

size_t debugPrint (const __FlashStringHelper * message);
size_t debugPrint (const String & message);
size_t debugPrint (const char * message);
size_t debugPrint (unsigned char message, int base = HEX);
size_t debugPrint (int message, int base = HEX);
size_t debugPrint (unsigned int message, int base = HEX);
size_t debugPrint (long message, int base = HEX);
size_t debugPrint (unsigned long message, int a = HEX);
size_t debugPrint (double message, int base = HEX);
size_t debugPrint (const Printable & message);

size_t debugPrintln ();
size_t debugPrintln (const __FlashStringHelper * message);
size_t debugPrintln (const String & message);
size_t debugPrintln (const char * message);
size_t debugPrintln (unsigned char message, int base = HEX);
size_t debugPrintln (int message, int base = HEX);
size_t debugPrintln (unsigned int message, int base = HEX);
size_t debugPrintln (long message, int base = HEX);
size_t debugPrintln (unsigned long message, int base = HEX);
size_t debugPrintln (double message, int base = HEX);
size_t debugPrintln (const Printable & message);

size_t debugPrint (const __FlashStringHelper * message)
{
	if (DEBUG_ENABLED)
		return Serial.print (message);

	return -1;
}

size_t debugPrint (const String & message)
{
	if (DEBUG_ENABLED)
		return Serial.print (message);

	return -1;
}

size_t debugPrint (const char * message)
{
	if (DEBUG_ENABLED)
		return Serial.print (message);

	return -1;
}

size_t debugPrint (unsigned char message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.print (message, base);

	return -1;
}

size_t debugPrint (int message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.print (message, base);

	return -1;
}

size_t debugPrint (unsigned int message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.print (message, base);

	return -1;
}

size_t debugPrint (long message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.print (message, base);

	return -1;
}

size_t debugPrint (unsigned long message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.print (message, base);

	return -1;
}

size_t debugPrint (double message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.print (message, base);

	return -1;
}

size_t debugPrint (const Printable & message)
{
	if (DEBUG_ENABLED)
		return Serial.print (message);

	return -1;
}

size_t debugPrintln ()
{
	if (DEBUG_ENABLED)
		return Serial.println();

	return -1;
}

size_t debugPrintln (const __FlashStringHelper * message)
{
	if (DEBUG_ENABLED)
		return Serial.println (message);

	return -1;
}

size_t debugPrintln (const String & message)
{
	if (DEBUG_ENABLED)
		return Serial.println (message);

	return -1;
}

size_t debugPrintln (const char * message)
{
	if (DEBUG_ENABLED)
		return Serial.println (message);

	return -1;
}

size_t debugPrintln (unsigned char message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.println (message, base);

	return -1;
}

size_t debugPrintln (int message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.println (message, base);

	return -1;
}

size_t debugPrintln (unsigned int message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.println (message, base);

	return -1;
}

size_t debugPrintln (long message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.println (message, base);

	return -1;
}

size_t debugPrintln (unsigned long message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.println (message, base);

	return -1;
}

size_t debugPrintln (double message, int base)
{
	if (DEBUG_ENABLED)
		return Serial.println (message, base);

	return -1;
}

size_t debugPrintln (const Printable & message)
{
	if (DEBUG_ENABLED)
		return Serial.println (message);

	return -1;
}
