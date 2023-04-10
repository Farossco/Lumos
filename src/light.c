#include "light.h"
#include <esp_log.h>
#include "light_strip.h"
#include "light_mode_task.h"
#include "temp_log_util.h"
#include "utils_c.h"

#define LIGHT_RGB_DEFAULT   RGB_WHITE
#define LIGHT_POWER_DEFAULT 255
#define LIGHT_SPEED_DEFAULT 63

static const char *TAG = "light";

static bool light_state;                                     /* If the leds are ON or OFF (True: ON / False: OFF) */
static uint8_t light_mode;                                   /* Current lighting mode */
static struct light_mode_data light_mode_data[LIGHT_MODE_N]; /* Light modes data */

const static char *light_mode_strings[LIGHT_MODE_N] = {
	"Continuous",
	"Flash",
	"Strobe",
	"Fade",
	"Smooth",
	"Dawn",
	"Sunset",
	"Start"
};

static void light_reset(void)
{
	for (uint8_t i = 0; i < LIGHT_MODE_N; i++) {
		light_mode_data[i].rgb   = LIGHT_RGB_DEFAULT;   /* Initializing rgbs to their default value */
		light_mode_data[i].power = LIGHT_POWER_DEFAULT; /* Initializing powers to their default value */
		light_mode_data[i].speed = LIGHT_SPEED_DEFAULT; /* Initializing speeds to their default value */
	}
}

static void on_light_mode_task_end(uint8_t mode)
{
	switch (mode) {
	case LIGHT_MODE_DAWN:
		/* Transferring RGB value and power from Dawn to continuous */
		light_mode_data[LIGHT_MODE_CONTINUOUS].rgb   = light_mode_data[LIGHT_MODE_DAWN].rgb;
		light_mode_data[LIGHT_MODE_CONTINUOUS].power = light_mode_data[LIGHT_MODE_DAWN].power;
		light_mode_set(LIGHT_MODE_CONTINUOUS);
		break;

	case LIGHT_MODE_SUNSET:
		light_state_set(LIGHT_OFF);
		light_mode_set(LIGHT_MODE_CONTINUOUS);
		break;

	case LIGHT_MODE_START:
		light_state_set(LIGHT_OFF);
		light_mode_set(LIGHT_MODE_CONTINUOUS);
		break;

	default:
		break;
	}
}

static struct light_mode_task_callbacks light_mode_task_callbacks = {
	.on_mode_task_end = on_light_mode_task_end
};

void light_color_set(rgb_t rgb, uint8_t mode)
{
	if (mode == LIGHT_MODE_CURRENT) {
		mode = light_mode;
	}

	if (mode > LIGHT_MODE_MAX) {
		return;
	}

	light_mode_data[mode].rgb = rgb;
}

void light_power_set(uint8_t power, uint8_t mode)
{
	if (mode == LIGHT_MODE_CURRENT) {
		mode = light_mode;
	}

	if (mode > LIGHT_MODE_MAX) {
		return;
	}

	light_mode_data[mode].power = power;
}

void light_speed_set(uint8_t speed, uint8_t mode)
{
	if (mode == LIGHT_MODE_CURRENT) {
		mode = light_mode;
	}

	if (mode > LIGHT_MODE_MAX) {
		return;
	}

	light_mode_data[mode].speed = speed;
}

void light_mode_set(uint8_t mode)
{
	if (mode > LIGHT_MODE_MAX) {
		ESP_LOGE(TAG, "Trying to set incorrect mode: %d", mode);
		return;
	}

	if (mode == light_mode) {
		return;
	}

	light_mode = mode;

	/* if the lights are on */
	if (light_state_get()) {
		/* Restart the light mode */
		light_mode_task_start(light_mode, &light_mode_data[light_mode]);
	}
}

void light_state_set(bool state)
{
	if (light_state_get() == state) { /* No change to the light_state */
		return;
	}

	if (state == LIGHT_ON) {
		ESP_LOGV(TAG, "Turning lights ON");

		/* Start a new task for the current mode */
		light_mode_task_start(light_mode, &light_mode_data[light_mode]);
	} else {
		ESP_LOGV(TAG, "Turning lights OFF");

		/* Terminate the current light mode task */
		light_mode_task_stop();
	}

	/* Turning lights ON or OFF */
	light_state = state;
}

/* TODO: remove getter */

rgb_t light_color_get(uint8_t mode)
{
	return light_mode_data[mode].rgb;
}

uint8_t light_power_get(uint8_t mode)
{
	return light_mode_data[mode].power;
}

uint8_t light_speed_get(uint8_t mode)
{
	return light_mode_data[mode].speed;
}

uint8_t light_mode_get(void)
{
	return light_mode;
}

bool light_state_get(void)
{
	return light_state;
}

const char * light_mode_string_get(uint8_t mode)
{
	if (mode < ARRAY_SIZE(light_mode_strings)) {
		return light_mode_strings[mode];
	} else {
		return "Unknown";
	}
}

int light_init(void)
{
	int err;

	ESP_LOGI(TAG, "Initializing light");

	light_strip_init();

	light_mode_task_register_callbacks(&light_mode_task_callbacks);

	err = light_mode_task_init();
	if (err) {
		return err;
	}

	light_reset();

	light_mode_set(LIGHT_MODE_START);
	light_state_set(LIGHT_ON);

	ESP_LOGI(TAG, "Light initialized");

	return 0;
}
