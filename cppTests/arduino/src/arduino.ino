#include "Global.h"
#include "Logger.h"
#include "Infrared.h"
#include "SdCard.h"
#include "VariableChange.h"
#include "ArduinoSerial.h"
#include <Time.h>

void setup ()
{
	setTime (1512833569);

	serial.begin (DEBUG_BAUD_RATE);

	global.begin();

	Log.begin (&Serial, 250000, LEVEL_VERBOSE);

	infrared.begin (PIN_LED_INFRARED);

	sd.begin (PIN_SD_CS);

	variableChange.begin();
}

void loop ()
{
	global.light();

	infrared.read();

	//variableChange.check();

	serial.read();
}
