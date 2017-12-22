#include <Time.h>
#include "Global.h"
#include "Logger.h"
#include "Infrared.h"
#include "SdCard.h"
#include "VariableChange.h"
#include "ArduinoSerial.h"
#include "Alarms.h"
#include "Mods.h"


void setup ()
{
	serial.init (DEBUG_BAUD_RATE, ESP_BAUD_RATE);

	sd.init (PIN_SD_CS);

	Log.init (&Serial, LEVEL_VERBOSE);

	serial.waitForTime();

	infrared.init (PIN_LED_INFRARED);

	// init DF player

	mods.init();

	global.init();

	variableChange.init();

	alarms.initAll();

	variableChange.sendInfo();
}

void loop ()
{
	Alarm.delay (0);

	variableChange.check();

	// read claps

	infrared.read();

	serial.read();

	mods.action();

	global.light();
}
