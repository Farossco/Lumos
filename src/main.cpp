#include <esp_err.h>
#include <esp_log.h>
#include "SdCard.h"
#include "uart_com.h"
#include "Alarms.h"
#include "light.h"
#include "Sound.h"
#include "network/wifi_com.h"
#include "memory.h"
#include "temp_log_util.h"
#include "utils_c.h"

static const char *TAG = "main";

void setup()
{
	esp_err_t err;

	err = memory_init();
	if (err) {
		ESP_LOGE(TAG, "Failed to init memory: ", err2str(err));
		return;
	}

	uart_com_init();

	light_init();

	wifi_com_init();

	/* sd.init(); / * SD SPI needs to be initialized after the light strip SPI * / */

	/* infrared.init(); */

	/* sound.init (Serial2); */

	/* alarms.init(); */
}

void loop()
{
	vTaskDelete(NULL);
	/* sd.action(); */

	/* infrared.read(); */

	/* sound.action(); */

	/* alarms.action(); */
}
