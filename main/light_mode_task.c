/**
 * @file light_mode_task.cpp
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-04-08
 *
 * @brief
 */

#include "light_mode_task.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "light.h"
#include "peripherals/light_strip.h"
#include "com/wifi_com.h"
#include "lib8tion.h"
#include "color.h"
#include "utils.h"

#define LIGHT_MODE_START_DONE_TAIL_LENGTH LIGHT_STRIP_HALF_LENGTH

static const char *TAG = "light_mode_task";

static TaskHandle_t light_mode_task_handle;
static QueueHandle_t light_mode_task_queue;
static volatile bool light_mode_start_done;

static const struct light_mode_task_callbacks *callbacks;

static void light_mode_task_create(uint8_t mode, struct light_mode_data *data);
static void light_mode_task_create_shutdown(void);

static void light_mode_task_delay(uint32_t delay_ms)
{
	struct light_mode_task_queue_data queue_data;

	if (xQueueReceive(light_mode_task_queue, &queue_data, delay_ms / portTICK_PERIOD_MS)) {
		/* Message has been sent to the queue */

		if (queue_data.shutdown) {
			light_mode_task_create_shutdown();
		} else {
			light_mode_task_create(queue_data.mode, queue_data.mode_data);
		}

		/* Self delete */
		ESP_LOGV(TAG, "Self destructing, handle: %p", xTaskGetCurrentTaskHandle());

		vTaskDelete(NULL);
	} else {
		/* Queue timed out, nothing to do */
	}
}

static void light_mode_task_update_and_delay(uint32_t delay_ms)
{
	light_strip_update();

	light_mode_task_delay(delay_ms);
}

/**
 * @brief Task running when the lights are off
 *
 * This periodically sends SPI commands to turn le LEDs off
 * to mitigate potential parasite signals.
 *
 * @param arg Task parameters - Unused
 */
static void light_mode_shutdown_task(void *arg)
{
	light_strip_color_all_set(RGB_BLACK, 0);

	while (true) {
		light_mode_task_update_and_delay(1000);
	}
}

static void light_mode_continuous_task(void *arg)
{
	struct light_mode_data *data = arg;

	ESP_LOGI(TAG, "Entering Continuous mode");

	while (true) {
		light_strip_color_all_set(data->rgb, data->power);

		light_mode_task_update_and_delay(100);
	}
}

/* Flash mode */
static void light_mode_flash_task(void *arg)
{
	struct light_mode_data *data = arg;

	ESP_LOGI(TAG, "Entering Flash mode");

	while (true) {
		light_strip_color_all_set(RGB_RED, data->power);

		light_mode_task_update_and_delay(1000U - data->speed * 10);

		light_strip_color_all_set(RGB_GREEN, data->power);

		light_mode_task_update_and_delay(1000U - data->speed * 10);

		light_strip_color_all_set(RGB_BLUE, data->power);

		light_mode_task_update_and_delay(1000U - data->speed * 10);
	}
}

/* Strobe mode */
static void light_mode_strobe_task(void *arg)
{
	struct light_mode_data *data = arg;

	ESP_LOGI(TAG, "Entering Strobe mode");

	while (true) {
		light_strip_color_all_set(data->rgb, data->power);

		light_mode_task_update_and_delay(1000U - data->speed * 10);

		light_strip_color_all_set(RGB_BLACK, 0);

		light_mode_task_update_and_delay(1000U - data->speed * 10);
	}
}

/* Fade Mode */
static void light_mode_fade_task(void *arg)
{
	struct light_mode_data *data = arg;
	uint8_t state                = 1;
	uint16_t counter             = 0; /* uint16_t for calculation */

	ESP_LOGI(TAG, "Entering Fade mode");

	while (true) {
		light_strip_color_all_set(data->rgb, (uint8_t)((counter * data->power) / 255));

		if (state)
			counter++;
		else
			counter--;

		if (counter == 255)
			state = 0;
		else if (counter == 0)
			state = 1;

		light_mode_task_update_and_delay((110U - data->speed) / 5);
	}
}

/* Smooth Mode */
static void light_mode_smooth_task(void *arg)
{
	struct light_mode_data *data = arg;
	rgb_t rgb                    = { { 255 }, { 0 }, { 0 } };
	uint8_t state                = 0;

	ESP_LOGI(TAG, "Entering Smooth mode");

	while (true) {
		light_strip_color_all_set(rgb, data->power);

		switch (state) {
		case 0: if (++rgb.g == 0xFF) state = 1; break;
		case 1: if (--rgb.r == 0x00) state = 2; break;
		case 2: if (++rgb.b == 0xFF) state = 3; break;
		case 3: if (--rgb.g == 0x00) state = 4; break;
		case 4: if (++rgb.r == 0xFF) state = 5; break;
		case 5: if (--rgb.b == 0x00) state = 0; break;
		}

		light_mode_task_update_and_delay((1000U - data->speed * 10) / 8);
	}
}

/* Dawn Mode */
static void light_mode_dawn_task(void *arg)
{
	struct light_mode_data *data = arg;
	int16_t counter1             = 0;
	int16_t counter2             = 100;
	uint32_t step                = 0;

	/* uint32_t step                = alarms.getDawnDuration() * 1000.0 / (LIGHT_STRIP_HALF_LENGTH * 100); */

	if (step == 0) {
		step = 1;
	}

	/* ESP_LOGI(TAG, "Entering Dawn mode for %dmin, %ds", alarms.getDawnDuration().minute(), alarms.getDawnDuration().second()); */
	ESP_LOGI(TAG, "Entering Dawn mode for %dmin, %ds", 0, 0);

	light_strip_color_all_set(RGB_BLACK, 0);

	while (counter2 > 0) {
		light_strip_color_set(LIGHT_STRIP_HALF_LENGTH + counter1, data->rgb, data->power / counter2);
		light_strip_color_set(LIGHT_STRIP_HALF_LENGTH - counter1, data->rgb, data->power / counter2);

		if (counter1++ >= LIGHT_STRIP_HALF_LENGTH) {
			counter1 = 0;

			counter2--;
		}

		light_mode_task_update_and_delay(step);
	}

	if (callbacks && callbacks->on_mode_task_end) {
		callbacks->on_mode_task_end(LIGHT_MODE_DAWN);
	} else {
		ESP_LOGE(TAG, "light_mode callback not defined");
	}

	/* Wait for a task creation request */
	while (true) {
		light_mode_task_delay(portMAX_DELAY);
	}
}

/* Sunset Mode */
static void light_mode_sunset_task(void *arg)
{
	struct light_mode_data *data = arg;
	int32_t counter1             = 0;
	int32_t counter2             = 1;

	/* ESP_LOGI(TAG, "Entering Sunset mode for %dmin, %ds", alarms.getDawnDuration().minute(), alarms.getDawnDuration().second()); */
	ESP_LOGI(TAG, "Entering Sunset mode for %dmin, %ds", 0, 0);

	light_strip_color_all_set(data->rgb, data->power);

	/* light_mode_task_update_and_delay(alarms.getSunsetDuration().value()); */
	light_mode_task_update_and_delay(0);

	/* ESP_LOGV(TAG, "Starting to shut down. Completely off in %dmin, %ds", alarms.getSunsetDecreaseTime().minute(), alarms.getSunsetDecreaseTime().second()); */
	ESP_LOGV(TAG, "Starting to shut down. Completely off in %dmin, %ds", 0, 0);

	while (counter2 < 100) {
		light_strip_color_set(counter1, data->rgb, data->power / counter2);
		light_strip_color_set(LIGHT_STRIP_LENGTH - counter1, data->rgb, data->power / counter2);

		if (counter1++ >= LIGHT_STRIP_HALF_LENGTH) {
			counter1 = 0;

			counter2++;
		}

		/* light_mode_task_update_and_delay(alarms.getSunsetDecreaseTime() * (1000.0 / (LIGHT_STRIP_HALF_LENGTH * 100))); */
		light_mode_task_update_and_delay(0);
	}

	if (callbacks && callbacks->on_mode_task_end) {
		callbacks->on_mode_task_end(LIGHT_MODE_SUNSET);
	} else {
		ESP_LOGE(TAG, "light_mode callback not defined");
	}

	/* Wait for a task creation request */
	while (true) {
		light_mode_task_delay(portMAX_DELAY);
	}
}

static void light_mode_start(void *arg)
{
	struct light_mode_data *data = arg;
	int32_t counter              = 0;
	uint8_t state                = 0;
	rgb_t rainbow[LIGHT_STRIP_HALF_LENGTH + 1];

	for (uint8_t i = 0; i < LIGHT_STRIP_HALF_LENGTH + 1; i++)
		rainbow[i] = hsv2rgb_rainbow((hsv_t) { { (uint8_t)(i * 255 / (LIGHT_STRIP_HALF_LENGTH + 1)) }, { 255 }, { 255 } });

	while (!light_mode_start_done) {
		counter = 0;
		state   = 0;

		while (counter >= 0) {
			for (uint8_t i = 0; i < LIGHT_STRIP_LENGTH; i++) {
				if (abs(i - LIGHT_STRIP_HALF_LENGTH) < counter) {
					light_strip_color_set(i, rainbow[LIGHT_STRIP_HALF_LENGTH - abs(i - LIGHT_STRIP_HALF_LENGTH)], data->power);
				} else {
					light_strip_color_set(i, RGB_BLACK, 0);
				}
			}

			light_mode_task_update_and_delay(abs(sin8(counter * (8 * 16) / LIGHT_STRIP_LENGTH + 64) - 80) / 7);

			if (counter > LIGHT_STRIP_HALF_LENGTH) {
				state = 1;
			} else if (counter < 0) {
				state = 0;
			}

			if (state) {
				counter--;
			} else {
				counter++;
			}
		}
	}

	counter = 0;

	while (counter < LIGHT_STRIP_HALF_LENGTH + LIGHT_MODE_START_DONE_TAIL_LENGTH + 2) {
		for (int8_t i = 0; i < LIGHT_STRIP_LENGTH; i++)
			if (counter > abs(i - LIGHT_STRIP_HALF_LENGTH) && abs(i - LIGHT_STRIP_HALF_LENGTH) >= counter - LIGHT_MODE_START_DONE_TAIL_LENGTH)
				light_strip_color_set(i, rainbow[LIGHT_STRIP_HALF_LENGTH - abs(i - LIGHT_STRIP_HALF_LENGTH)], data->power);
			else
				light_strip_color_set(i, RGB_BLACK, 0);

		light_mode_task_update_and_delay(15);

		counter++;
	}

	if (callbacks && callbacks->on_mode_task_end) {
		callbacks->on_mode_task_end(LIGHT_MODE_START);
	} else {
		ESP_LOGE(TAG, "light_mode callback not defined");
	}

	/* Wait for a task creation request */
	while (true) {
		light_mode_task_delay(portMAX_DELAY);
	}
}

static const TaskFunction_t light_mode_tasks[] = {
	light_mode_continuous_task,
	light_mode_flash_task,
	light_mode_strobe_task,
	light_mode_fade_task,
	light_mode_smooth_task,
	light_mode_dawn_task,
	light_mode_sunset_task,
	light_mode_start
};

static void light_mode_task_create(uint8_t mode, struct light_mode_data *data)
{
	if (mode >= ARRAY_SIZE(light_mode_tasks) || !light_mode_tasks[mode]) {
		ESP_LOGE(TAG, "No handler for mode %d", mode);
		return;
	}

	xTaskCreatePinnedToCore(light_mode_tasks[mode], "LightModeTask",
	                        10240, data, 1, &light_mode_task_handle, 0);

	ESP_LOGD(TAG, "Light mode task started for %d, handle: %p", mode, light_mode_task_handle);
}

static void light_mode_task_create_shutdown(void)
{
	xTaskCreatePinnedToCore(light_mode_shutdown_task, "LightModeShutdownTask",
	                        4096, NULL, 1, &light_mode_task_handle, 0);

	ESP_LOGD(TAG, "Shutdown mode task started, handle: %p", light_mode_task_handle);
}

void light_mode_task_start(uint8_t mode, struct light_mode_data *data)
{
	if (light_mode_task_handle == NULL) {
		ESP_LOGE(TAG, "Light mode is not initialized!");
		return;
	}

	struct light_mode_task_queue_data queue_data = {
		.shutdown  = false,
		.mode      = mode,
		.mode_data = data
	};

	xQueueSendToBack(light_mode_task_queue, &queue_data, 0);

	ESP_LOGD(TAG, "Light mode task creation request sent, mode: %d", mode);
}

void light_mode_task_stop(void)
{
	if (light_mode_task_handle == NULL) {
		ESP_LOGE(TAG, "Light mode is not initialized!");
		return;
	}

	struct light_mode_task_queue_data data = {
		.shutdown = true
	};

	xQueueSendToBack(light_mode_task_queue, &data, 0);

	ESP_LOGD(TAG, "Light mode task deletion request sent");
}

void light_mode_task_register_callbacks(const struct light_mode_task_callbacks *cbks)
{
	callbacks = cbks;
}

static void on_wifi_com_connected()
{
	light_mode_start_done = true;
}

static const struct wifi_com_conn_callbacks wifi_com_conn_callbacks = {
	.on_connected = on_wifi_com_connected
};

int light_mode_task_init(void)
{
	light_mode_task_queue = xQueueCreate(1, sizeof(struct light_mode_task_queue_data));

	if (light_mode_task_queue == NULL) {
		ESP_LOGE(TAG, "Could not create queue!");
		return ESP_ERR_NO_MEM;
	}

	wifi_com_register_conn_callbacks(&wifi_com_conn_callbacks);

	light_mode_task_create_shutdown();

	return ESP_OK;
}
