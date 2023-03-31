/**
 * @file light_mode.cpp
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-03-31
 *
 * @brief
 */

#include "light_mode.h"
#include <ArduinoLogger.h>
#include "light_strip.h"
#include "Alarms.h" /* TODO: Remove */

static struct light_mode_callbacks callbacks;
static TaskHandle_t light_mode_task_handle;
static TaskHandle_t light_mode_shutdown_task_handle;

static void update_and_delay(struct light_mode_data *data, uint32_t delay_ms)
{
	light_strip_update(data->power);

	delay(delay_ms);
}

static void light_mode_shutdown_task(void *pvParameters)
{
	while (true) {
		light_strip_color_all_set(0);
		light_strip_update();

		delay(100);
	}
}

static void light_mode_continuous_task(void *pvParameters)
{
	struct light_mode_data *data = (light_mode_data *)pvParameters;

	inf << "Entering Continuous mode" << dendl;

	while (true) {
		light_strip_color_all_set(data->rgb);

		update_and_delay(data, 1000);
	}
}

/* Flash mode */
static void light_mode_flash_task(void *pvParameters)
{
	struct light_mode_data *data = (light_mode_data *)pvParameters;

	inf << "Entering Flash mode" << dendl;

	while (true) {
		light_strip_color_all_set(0xFF0000);

		update_and_delay(data, 1000U - data->speed * 10);

		light_strip_color_all_set(0x00FF00);

		update_and_delay(data, 1000U - data->speed * 10);

		light_strip_color_all_set(0x0000FF);

		update_and_delay(data, 1000U - data->speed * 10);
	}
}

/* Strobe mode */
static void light_mode_strobe_task(void *pvParameters)
{
	struct light_mode_data *data = (light_mode_data *)pvParameters;

	inf << "Entering Strobe mode" << dendl;

	while (true) {
		light_strip_color_all_set(data->rgb);

		update_and_delay(data, 1000U - data->speed * 10);

		light_strip_color_all_set(0);

		update_and_delay(data, 1000U - data->speed * 10);
	}
}

/* Fade Mode */
static void light_mode_fade_task(void *pvParameters)
{
	struct light_mode_data *data = (light_mode_data *)pvParameters;
	uint8_t state                = 0;
	int32_t counter              = 0;

	inf << "Entering Fade mode" << dendl;

	while (true) {
		light_strip_color_all_set(data->rgb * (counter / 255.));

		if (counter >= 255)
			state = 0;
		else if (counter <= 0)
			state = 1;

		if (state)
			counter++;
		else
			counter--;

		update_and_delay(data, (1000U - data->speed * 10) / 50);
	}
}

/* Smooth Mode */
static void light_mode_smooth_task(void *pvParameters)
{
	struct light_mode_data *data = (light_mode_data *)pvParameters;
	uint8_t state                = 0;
	uint8_t red                  = 0xFF;
	uint8_t green                = 0;
	uint8_t blue                 = 0;

	inf << "Entering Smooth mode" << dendl;

	while (true) {
		light_strip_color_all_set(LightRgb(red, green, blue));

		switch (state) {
		case 0: if (++green == 0xFF) state = 1; break;
		case 1: if (--red == 0x00) state = 2; break;
		case 2: if (++blue == 0xFF) state = 3; break;
		case 3: if (--green == 0x00) state = 4; break;
		case 4: if (++red == 0xFF) state = 5; break;
		case 5: if (--blue == 0x00) state = 0; break;
		}

		update_and_delay(data, (1000U - data->speed * 10) / 8);
	}
}

/* Dawn Mode */
static void light_mode_dawn_task(void *pvParameters)
{
	struct light_mode_data *data = (light_mode_data *)pvParameters;
	int16_t counter1             = 0;
	int16_t counter2             = 1;
	uint32_t step                = alarms.getDawnDuration() * (1000.0 / (LIGHT_STRIP_HALF_LENGTH * 255));

	inf << "Entering Dawn mode for " << alarms.getDawnDuration() << dendl;

	light_strip_color_all_set(0);

	while (counter2 < 255) {
		light_strip_color_set(LIGHT_STRIP_HALF_LENGTH + counter1, data->rgb * (counter2 / 255.));
		light_strip_color_set(LIGHT_STRIP_HALF_LENGTH - counter1, data->rgb * (counter2 / 255.));

		if (counter1++ >= LIGHT_STRIP_HALF_LENGTH) {
			counter1 = 0;

			counter2++;
		}

		update_and_delay(data, step);
	}

	/* Transfer current mode data to default mode */
	/* light_mode_data[LightMode::continuous] = light_mode_data[light_mode]; */

	if (callbacks.on_light_mode_end) {
		/* light_mode_set(LightMode::continuous); / * Leaving the mode * / */
		callbacks.on_light_mode_end();
	}

	/* Keep the task alive and doing nothing until it gets terminated */
	vTaskDelay(portMAX_DELAY);
}

/* Sunset Mode */
static void light_mode_sunset_task(void *pvParameters)
{
	struct light_mode_data *data = (light_mode_data *)pvParameters;
	int32_t counter1             = 0;
	int32_t counter2             = 255;

	inf << "Entering Sunset mode for " << alarms.getSunsetDuration() << dendl;

	light_strip_color_all_set(data->rgb);

	update_and_delay(data, alarms.getSunsetDuration().value());

	trace << "Starting to shut down. Completely off in " << alarms.getSunsetDecreaseTime() << dendl;

	while (counter2 > 0) {
		light_strip_color_set(counter1, data->rgb * (counter2 / 255.));
		light_strip_color_set(LIGHT_STRIP_LENGTH - counter1, data->rgb * (counter2 / 255.));

		if (counter1++ >= LIGHT_STRIP_HALF_LENGTH) {
			counter1 = 0;

			counter2--;
		}

		update_and_delay(data, alarms.getSunsetDecreaseTime() * (1000.0 / (LIGHT_STRIP_HALF_LENGTH * 255)));
	}

	if (callbacks.on_light_mode_end) {
		/* light_on = LightOnOff(false); / * Shutting down the lights * / */
		/* light_mode_set(LightMode::continuous); / * Leaving the mode * / */
		callbacks.on_light_mode_end();
	}

	/* Keep the task alive and doing nothing until it gets terminated */
	vTaskDelay(portMAX_DELAY);
}

static const TaskFunction_t light_mode_tasks[] = { light_mode_continuous_task,
	                                               light_mode_flash_task,
	                                               light_mode_strobe_task,
	                                               light_mode_fade_task,
	                                               light_mode_smooth_task,
	                                               light_mode_dawn_task,
	                                               light_mode_sunset_task };

static void light_mode_task_create(LightMode mode, struct light_mode_data *data)
{
	if (mode >= ARRAY_SIZE(light_mode_tasks) || !light_mode_tasks[mode]) {
		err << "No handler for mode " << mode << endl;
		return;
	}

	if (light_mode_task_handle) {
		err << "Trying to create a light mode task while another runs!" << endl;
		light_mode_stop();
	}

	xTaskCreatePinnedToCore(light_mode_tasks[mode], "LightModeTask",
	                        10240, data, 1, &light_mode_task_handle, 0);

	verb << "Light mode task started, handle: " << hex << (uint32_t)light_mode_task_handle << endl;
}

static void light_mode_task_delete(void)
{
	if (!light_mode_task_handle) {
		err << "Trying to delete an unexisting light mode task!" << endl;
		return;
	}

	vTaskDelete(light_mode_task_handle);
	light_mode_task_handle = NULL;

	verb << "Light mode task deleted" << endl;
}

static void light_mode_shutdown_task_create(void)
{
	if (light_mode_shutdown_task_handle) {
		err << "Trying to create a shutdown task while another runs!" << endl;
		light_mode_stop();
	}

	xTaskCreatePinnedToCore(light_mode_shutdown_task, "LightModeShutdownTask",
	                        4096, NULL, 1, &light_mode_shutdown_task_handle, 0);

	verb << "Shutdown task started, handle: " << hex << (uint32_t)light_mode_shutdown_task_handle << endl;
}

static void light_mode_shutdown_task_delete(void)
{
	if (!light_mode_shutdown_task_handle) {
		err << "Trying to delete an unexisting shutdown task!" << endl;
		return;
	}

	vTaskDelete(light_mode_shutdown_task_handle);
	light_mode_shutdown_task_handle = NULL;

	verb << "Shutdown task deleted" << endl;
}

void light_mode_init()
{
	light_mode_shutdown_task_create();
}

void light_mode_start(LightMode mode, struct light_mode_data *data)
{
	light_mode_shutdown_task_delete();

	light_mode_task_create(mode, data);
}

void light_mode_stop(void)
{
	light_mode_task_delete();

	light_mode_shutdown_task_create();
}

void light_mode_restart(LightMode mode, struct light_mode_data *data)
{
	light_mode_task_delete();
	light_mode_task_create(mode, data);
}
