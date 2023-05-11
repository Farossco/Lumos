#include "light.h"
#include "peripherals/light_strip.h"
#include "light_mode_task.h"
#include "utils.h"
#include "json.h"

#define LIGHT_RGB_DEFAULT   RGB_WHITE
#define LIGHT_POWER_DEFAULT LIGHT_POWER_MAX
#define LIGHT_POWER_MAX     255
#define LIGHT_SPEED_DEFAULT 63
#define LIGHT_SPEED_MAX     100

static const char *TAG = "light";

static bool light_state;                                     /* If the leds are ON or OFF (True: ON / False: OFF) */
static uint8_t light_mode;                                   /* Current lighting mode */
static struct light_mode_data light_mode_data[LIGHT_MODE_N]; /* Light modes data */

#define WEB_COLOR_WIDTH  6 /* The width of the web interface's color panel */
#define WEB_COLOR_HEIGHT 4 /* The height of the web interface's color panel */

const int web_color_list[WEB_COLOR_HEIGHT][WEB_COLOR_WIDTH] = {
	{ 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF },
	{ 0xFF0000, 0xFF5500, 0xFFAA00, 0xFFFF00, 0xAAFF00, 0x55FF00 },
	{ 0x00FF00, 0x00FF55, 0x00FFAA, 0x00FFFF, 0x00AAFF, 0x0055FF },
	{ 0x0000FF, 0x5500FF, 0xAA00FF, 0xFF00FF, 0xFF00AA, 0xFF0055 }
};

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

const char * light_mode_string_get(uint8_t mode)
{
	if (mode < ARRAY_SIZE(light_mode_strings)) {
		return light_mode_strings[mode];
	} else {
		return "Unknown";
	}
}

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

esp_err_t light_color_set(rgb_t rgb, uint8_t mode)
{
	if (mode == LIGHT_MODE_CURRENT) {
		mode = light_mode;
	}

	if (mode > LIGHT_MODE_MAX) {
		return ESP_ERR_INVALID_ARG;
	}

	light_mode_data[mode].rgb = rgb;

	ESP_LOGI(TAG, "Light color for %s (%d) set to %lX", light_mode_string_get(mode), mode, rgb_to_code(rgb));

	return ESP_OK;
}

esp_err_t light_power_set(uint8_t power, uint8_t mode)
{
	if (mode == LIGHT_MODE_CURRENT) {
		mode = light_mode;
	}

	if (mode > LIGHT_MODE_MAX) {
		return ESP_ERR_INVALID_ARG;
	}

	if (power > LIGHT_POWER_MAX) {
		return ESP_ERR_INVALID_ARG;
	}

	light_mode_data[mode].power = power;

	ESP_LOGI(TAG, "Light power for %s (%d) set to %d", light_mode_string_get(mode), mode, power);

	return ESP_OK;
}

esp_err_t light_speed_set(uint8_t speed, uint8_t mode)
{
	if (mode == LIGHT_MODE_CURRENT) {
		mode = light_mode;
	}

	if (mode > LIGHT_MODE_MAX) {
		return ESP_ERR_INVALID_ARG;
	}

	if (speed > LIGHT_SPEED_MAX) {
		return ESP_ERR_INVALID_ARG;
	}

	light_mode_data[mode].speed = speed;

	ESP_LOGI(TAG, "Light speed for %s (%d) set to %d", light_mode_string_get(mode), mode, speed);

	return ESP_OK;
}

esp_err_t light_mode_set(uint8_t mode)
{
	if (mode > LIGHT_MODE_MAX) {
		ESP_LOGE(TAG, "Trying to set incorrect mode: %d", mode);
		return ESP_ERR_INVALID_ARG;
	}

	if (mode == light_mode) {
		return ESP_OK;
	}

	light_mode = mode;

	/* if the lights are on */
	if (light_state == LIGHT_ON) {
		/* Restart the light mode */
		light_mode_task_start(light_mode, &light_mode_data[light_mode]);
	}

	ESP_LOGI(TAG, "Light mode set to %s (%d)", light_mode_string_get(mode), mode);

	return ESP_OK;
}

esp_err_t light_state_set(bool state)
{
	if (light_state == state) { /* No change to the light_state */
		return ESP_OK;
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

	ESP_LOGI(TAG, "Light state set to %s", state == LIGHT_ON ? "ON" : "OFF");

	return ESP_OK;
}

static esp_err_t on_json_data_gen(json_callback_ctx_t *ctx)
{
	int light_mode_data_array[LIGHT_MODE_N];

	JSON_GEN_ADD_BOOL("On", light_state);

	JSON_GEN_ADD_INT("Mode", light_mode);

	for (uint8_t i = 0; i < LIGHT_MODE_N; i++) {
		light_mode_data_array[i] = rgb_to_code(light_mode_data[i].rgb);
	}
	JSON_GEN_ADD_INT_ARRAY("Rgb", light_mode_data_array, LIGHT_MODE_N);

	for (uint8_t i = 0; i < LIGHT_MODE_N; i++) {
		light_mode_data_array[i] = light_mode_data[i].power;
	}
	JSON_GEN_ADD_INT_ARRAY("Power", light_mode_data_array, LIGHT_MODE_N);

	for (uint8_t i = 0; i < LIGHT_MODE_N; i++) {
		light_mode_data_array[i] = light_mode_data[i].speed;
	}
	JSON_GEN_ADD_INT_ARRAY("Speed", light_mode_data_array, LIGHT_MODE_N);

	return ESP_OK;
}

static esp_err_t on_json_res_colors_gen(json_callback_ctx_t *ctx)
{
	for (uint8_t i = 0; i < WEB_COLOR_HEIGHT; i++) {
		JSON_GEN_ARRAY_ADD_INT_ARRAY(web_color_list[i], WEB_COLOR_WIDTH);
	}

	return ESP_OK;
}

static esp_err_t on_json_res_gen(json_callback_ctx_t *ctx)
{
	JSON_GEN_ADD_STRING_ARRAY("ModeNames", light_mode_strings, LIGHT_MODE_N);

	JSON_GEN_ADD_GENERIC_ARRAY("Colors", on_json_res_colors_gen);

	return ESP_OK;
}

static struct json_sub_data json_data_sub = {
	.sub_name         = "Light",
	.json_generate_cb = on_json_data_gen
};

static struct json_sub_data json_res_sub = {
	.sub_name         = "Light",
	.json_generate_cb = on_json_res_gen
};

esp_err_t light_init(void)
{
	esp_err_t err;

	ESP_LOGI(TAG, "Initializing light");

	light_strip_init();

	light_mode_task_register_callbacks(&light_mode_task_callbacks);

	err = light_mode_task_init();
	if (err) {
		ESP_LOGE(TAG, "Failed to init light_mode_task: %s", err2str(err));
		return err;
	}

	err = json_subscribe_to(JSON_TYPE_DATA, &json_data_sub);
	if (err) {
		ESP_LOGE(TAG, "Failed to subscribe to JSON data: %s", err2str(err));
		return err;
	}

	err = json_subscribe_to(JSON_TYPE_RES, &json_res_sub);
	if (err) {
		ESP_LOGE(TAG, "Failed to subscribe to JSON res: %s", err2str(err));
		return err;
	}

	light_reset();

	light_mode_set(LIGHT_MODE_START);
	light_state_set(LIGHT_ON);

	ESP_LOGI(TAG, "Light initialized");

	return ESP_OK;
}
