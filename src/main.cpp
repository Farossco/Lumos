#include <Time.h>
#include "SdCard.h"
#include "VariableChange.h"
#include "SerialCom.h"
#include "Alarms.h"
#include "Bluetooth.h"
#include "Light.h"
#include "Sound.h"
#include "ESPWifi.h"
#include "Infrared.h"

const boolean WAIT_FOR_TIME = true; // If the Arduino has to wait for time sync (if true, program will not start until time is synced)

#if defined(LUMOS_ARDUINO_MEGA) // Arduino code
void setup ()
{
	serial.init (ARDUINO_DEBUG_BAUD_RATE, COMM_BAUD_RATE);

	light.init();

	sd.init(); // SD SPI needs to be initialized after the light strip SPI

	infrared.init();

	sound.init (Serial3);

	alarms.init();

	variableChange.init();

	variableChange.sendData();
}

void loop ()
{
	variableChange.check();

	serial.receiveAndDecode();

	sd.action();

	infrared.read();

	light.action();

	sound.action();

	alarms.action();

	// bluetooth.action();
}

#endif // if defined(LUMOS_ARDUINO_MEGA)

#if defined(LUMOS_ESP32) // ESP8266 Code
void setup ()
{
	serial.init (ESP_DEBUG_BAUD_RATE, COMM_BAUD_RATE);

	wifi.init();

	wifi.getTime();

	serial.sendTime();

	serial.getData();
}

void loop ()
{
	serial.receiveAndDecode();

	wifi.receiveAndDecode();
}

#endif // if defined(LUMOS_ESP32)