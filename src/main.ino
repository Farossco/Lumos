#include <Time.h>
#include "Logger.h"
#include "SdCard.h"
#include "VariableChange.h"
#include "ArduinoSerial.h"
#include "Alarms.h"
#include "Bluetooth.h"
#include "Light.h"
#include "Sound.h"
#include "ESPSerial.h"
#include "Wifi.h"
#include "Infrared.h"

const boolean WAIT_FOR_TIME = true; // If the Arduino has to wait for time sync (if true, program will not start until time is synced)

const long ARDUINO_DEBUG_BAUD_RATE = 250000; // Arduino debug baud rate
const long COMM_BAUD_RATE          = 115200; // ESP8266 communication baud rate
const long ESP_DEBUG_BAUD_RATE     = 115200; // ESP8266 debug baud rate

#if defined(LUMOS_ARDUINO_MEGA) // Arduino code
void setup ()
{
	serial.init (ARDUINO_DEBUG_BAUD_RATE, COMM_BAUD_RATE);

	Log.init (&Serial, LEVEL_VERBOSE);

	light.init();

	if (WAIT_FOR_TIME)
		serial.waitForTime();

	// sd.init();

	Log.init (&Serial, LEVEL_VERBOSE, sd.getFile(), LEVEL_VERBOSE);

	infrared.init();

	sound.init (Serial3);

	variableChange.init();

	alarms.init();

	variableChange.sendInfo();

	// bluetooth.init();
}

void loop ()
{
	variableChange.check();

	serial.receiveAndDecode();

	// sd.cardTests();

	infrared.read();

	light.action();

	sound.action();

	alarms.action();

	// bluetooth.action();
}

#endif // if defined(LUMOS_ARDUINO_MEGA)

#if defined(LUMOS_ESP8266) // ESP8266 Code
void setup ()
{
	serial.init (ESP_DEBUG_BAUD_RATE, COMM_BAUD_RATE);

	Log.init (&Serial, LEVEL_VERBOSE);

	wifi.init();

	wifi.getTime();

	serial.sendTime();
}

void loop ()
{
	serial.receiveAndDecode();

	wifi.receiveAndDecode();
}

#endif // if defined(LUMOS_ESP8266)