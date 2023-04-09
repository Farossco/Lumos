#include <TimeLib.h>
#include "SdCard.h"
#include "uart_com.h"
#include "Alarms.h"
#include "light.h"
#include "Sound.h"
#include "wifi_com.h"
#include "Infrared.h"
#include "wifi_test.h"

void setup()
{
	/* uart_com_init(); */

	/* light_init(); */

	initialise_wifi();

	/* wifi_com_init(); */

	/* sd.init(); / * SD SPI needs to be initialized after the light strip SPI * / */

	/* infrared.init(); */

	/* sound.init (Serial2); */

	/* alarms.init(); */

	vTaskDelete(NULL);
}

void loop()
{
	/* sd.action(); */

	/* infrared.read(); */

	/* sound.action(); */

	/* alarms.action(); */
}
