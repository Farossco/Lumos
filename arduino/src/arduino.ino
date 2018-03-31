#include <Time.h>
#include "Logger.h"
#include "SdCard.h"
#include "VariableChange.h"
#include "ArduinoSerial.h"
#include "Alarms.h"
#include "Bluetooth.h"
#include "Light.h"

const long ESP_BAUD_RATE   = 250000; // ESP8266 communication baud rate
const long DEBUG_BAUD_RATE = 250000; // Debug baud rate
const long DFP_BAUD_RATE   = 9600;   // DFPlayer communication baud rate

void setup ()
{
	serial.init (DEBUG_BAUD_RATE, ESP_BAUD_RATE);

	Log.init (&Serial, LEVEL_VERBOSE);

	serial.waitForTime();

	sd.init();

	Log.init (&Serial, LEVEL_VERBOSE, sd.getFile(), LEVEL_VERBOSE);

	// infrared.init (PIN_LED_INFRARED);


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

	// read claps

	// infrared.read();

	serial.read();

	sd.cardTests();

	// bluetooth.action();

	light.action();
}
