#include <TimeLib.h>
#include "SdCard.h"
#include "serial_com.h"
#include "Alarms.h"
#include "light.h"
#include "Sound.h"
#include "wifi_com.h"
#include "Infrared.h"

void setup()
{
	serial_com_init(ESP_DEBUG_BAUD_RATE, 0);

	light_init();

	wifi_com_init();

	wifi_com_time_get();

	/* sd.init(); / * SD SPI needs to be initialized after the light strip SPI * / */

	/* infrared.init(); */

	/* sound.init (Serial2); */

	/* alarms.init(); */
}

void loop()
{
	serial_com_receive_and_decode();

	/* sd.action(); */

	/* infrared.read(); */

	/* sound.action(); */

	alarms.action();
}
