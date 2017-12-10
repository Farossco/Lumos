#include "ArduinoLog.h"
#include "SD.h"

/*!
 * This example sketch shows most of the features of the ArduinoLog library
 *
 */


int intValue1, intValue2;
long longValue1, longValue2;
bool boolValue1, boolValue2;
const char * charArray = "this is a string";
String stringValue1    = "this is a string";
float floatValue;
double doubleValue;

void setup ()
{
	Log.begin (&Serial, 19200, LEVEL_TRACE);

	Log.infonp ("\n");

	Log.info ("*****************************************\n"); // Info string with Newline
	Log.info ("************ Logging example ************\n"); // Info string in flash memory
	Log.info ("*****************************************\n"); // two info strings without newline

	Log.infonp ("\n");
}

void loop ()
{
	// set up some random variables
	intValue1   = random (100);
	intValue2   = random (10000);
	longValue1  = random (1000000);
	longValue2  = random (100000000);
	boolValue1  = random (2) == 0;
	boolValue2  = random (2) == 1;
	floatValue  = 12.34;
	doubleValue = 1234.56789;

	Log.info   ("Log as Info with integer values : %d, %d" "\n", intValue1, intValue2);
	Log.info   ("Log as Info with hex values     : %x, %X" "\n", intValue2, intValue2);
	Log.info   ("Log as Info with binary values  : %b, %B" "\n", intValue2, intValue2);
	Log.info   ("Log as Info with bool values    : %t, %T" "\n", boolValue1, boolValue2);
	Log.info   ("Log as Info with string value   : %s" "\n", stringValue1.c_str());
	Log.info   ("Log as Info with float value   : %F" "\n", floatValue);
	Log.info   ("Log as Info with double value   : %D" "\n", doubleValue);
	Log.trace    ("Log as Trace with bool value    : %T" "\n", boolValue1);
	Log.warning  ("Log as Warning with bool value  : %T" "\n", boolValue1);
	Log.error    ("Log as Error with bool value    : %T" "\n", boolValue1);
	delay (5000);
} // loop

void printTimestamp (Print * _logOutput)
{
	char c[12];

	sprintf (c, "%10lu ", millis());

	_logOutput->print (c);
}

void printNewline (Print * _logOutput)
{
	_logOutput->print ('\n');
}
