#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "com/uart_com.h"
#include "com/wifi_com.h"
#include "light.h"
#include "memory.h"
#include "utils.h"

static const char *TAG = "main";

void app_main(void)
{
	esp_err_t err;

	err = memory_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init memory: %s", err2str(err));
		return;
	}

	err = uart_com_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init uart_com: %s", err2str(err));
		return;
	}

	err = light_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init light: %s", err2str(err));
		return;
	}

	err = wifi_com_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init wifi_com: %s", err2str(err));
		return;
	}
}
