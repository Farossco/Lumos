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

void setup ()
{
	serial.init (ESP_DEBUG_BAUD_RATE, 0);

	wifi.init();

	wifi.getTime();

	sd.init(); // SD SPI needs to be initialized after the light strip SPI

	light.init();

	infrared.init();

	sound.init (Serial2);

	alarms.init();

	// variableChange.init();

	// variableChange.sendData();
}

void loop ()
{
	// variableChange.check();

	wifi.receiveAndDecode();

	serial.receiveAndDecode();

	sd.action();

	infrared.read();

	light.action();

	sound.action();

	alarms.action();

	// bluetooth.action();
}