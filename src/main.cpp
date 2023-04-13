#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "com/uart_com.h"
#include "com/wifi_com.h"
#include "light.h"
#include "memory.h"
#include "utils.h"

static const char *TAG = "main";

void setup()
{
	esp_err_t err;

	err = memory_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init memory: ", err2str(err));
		return;
	}

	err = uart_com_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init uart_com: ", err2str(err));
		return;
	}

	err = light_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init light: ", err2str(err));
		return;
	}

	err = wifi_com_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init wifi_com: ", err2str(err));
		return;
	}

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
