#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <settings.h>
#include <nvs_flash.h>
#include "alarms.h"
#include "com/uart_com.h"
#include "com/wifi_com.h"
#include "light.h"
#include "spiffs.h"
#include "utils.h"

static const char *TAG = "main";

void app_main(void)
{
	esp_err_t err;

	err = nvs_flash_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init nvs_flash: %s", esp_err_to_name(err));
		return;
	}

	err = settings_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init settings: %s", esp_err_to_name(err));
		return;
	}

	err = alarms_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init alarms: %s", esp_err_to_name(err));
		return;
	}

	err = spiffs_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init spiffs: %s", esp_err_to_name(err));
		return;
	}

	err = uart_com_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init uart_com: %s", esp_err_to_name(err));
		return;
	}

	err = light_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init light: %s", esp_err_to_name(err));
		return;
	}

	err = wifi_com_init();
	if (err) {
		ESP_LOGE(TAG, "FATAL: Failed to init wifi_com: %s", esp_err_to_name(err));
		return;
	}
}
