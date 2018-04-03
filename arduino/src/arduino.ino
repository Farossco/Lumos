#include <Time.h>
#include "Logger.h"
#include "SdCard.h"
#include "VariableChange.h"
#include "ArduinoSerial.h"
#include "Alarms.h"
#include "Bluetooth.h"
#include "Light.h"
#include "Sound.h"

const boolean WAIT_FOR_TIME = false; // If we have to wait for time sync (if true, program will not start until time is synced)

const long ESP_BAUD_RATE   = 250000; // ESP8266 communication baud rate
const long DEBUG_BAUD_RATE = 250000; // Debug baud rate

void setup ()
{
	light.lightAll (0x000000);

	serial.init (DEBUG_BAUD_RATE, ESP_BAUD_RATE);

	Log.init (&Serial, LEVEL_VERBOSE);

	if (WAIT_FOR_TIME)
		serial.waitForTime();

	sd.init();

	Log.init (&Serial, LEVEL_VERBOSE, sd.getFile(), LEVEL_VERBOSE);

	sound.init (Serial3);

	light.init();

	variableChange.init();

	alarms.initAll();

	variableChange.sendInfo();

	// bluetooth.init();
}

void loop ()
{
	Alarm.delay (0);

	variableChange.check();

	serial.receiveAndDecode();

	sd.cardTests();

	// bluetooth.action();

	light.action();

	sound.action();
}
