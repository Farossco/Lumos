#include "json.h"
#include <cJSON.h>
#include <esp_err.h>
#include "light.h"
#include "Resources.h"
#include "temp_log_util.h"
#include "utils_c.h"

static const char *TAG = "json";

static esp_err_t json_generate_data(char *buf, size_t size, bool pretty)
{
	cJSON *root, *level_1, *level_2;
	esp_err_t ret = 0;

	root = cJSON_CreateObject();
	if (!root) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	/******* Status *******/
	if (!cJSON_AddStringToObject(root, "Status", "Ok")) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	/******* Message *******/
	if (!cJSON_AddStringToObject(root, "Message", "")) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	/******* Light *******/
	level_1 = cJSON_AddObjectToObject(root, "Light");
	if (!level_1) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	/* -- On/Off -- */
	if (!cJSON_AddBoolToObject(level_1, "On", light_state_get())) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	/* -- Mode -- */
	if (!cJSON_AddNumberToObject(level_1, "Mode", light_mode_get())) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	/* -- Rgb -- */
	level_2 = cJSON_AddArrayToObject(level_1, "Rgb");
	if (!level_2) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	for (uint8_t mode; mode < LIGHT_MODE_N; mode++) {
		if (!cJSON_AddItemToArray(level_2, cJSON_CreateNumber(rgb_to_code(light_color_get(mode))))) {
			ESP_LOGE(TAG, "Failed to add item to Object/Array");
			ret = -ENOMEM;
			goto end;
		}
	}

	/* -- Power -- */
	level_2 = cJSON_AddArrayToObject(level_1, "Power");
	if (!level_2) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	for (uint8_t mode; mode < LIGHT_MODE_N; mode++) {
		if (!cJSON_AddItemToArray(level_2, cJSON_CreateNumber(light_power_get(mode)))) {
			ESP_LOGE(TAG, "Failed to add item to Object/Array");
			ret = -ENOMEM;
			goto end;
		}
	}

	/* -- Speed -- */
	level_2 = cJSON_AddArrayToObject(level_1, "Speed");
	if (!level_2) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	for (uint8_t mode; mode < LIGHT_MODE_N; mode++) {
		if (!cJSON_AddItemToArray(level_2, cJSON_CreateNumber(light_speed_get(mode)))) {
			ESP_LOGE(TAG, "Failed to add item to Object/Array");
			ret = -ENOMEM;
			goto end;
		}
	}

	/* ****** Sound ****** // */
	/* JsonObject rootSound = root.createNestedObject("Sound"); */

	/*
	 * rootSound["On"]     = sound.isOn();              / * -- On/Off -- // * /
	 * rootSound["Volume"] = sound.getVolume().value(); / * -- Volume -- // * /
	 * rootSound["Mode"]   = (uint8_t)sound.getMode();  / * -- Mode -- // * /
	 */


	/*
	 * / * ****** Alarms ****** // * /
	 * JsonObject rootAlarms = root.createNestedObject("Alarms");
	 */

	/*
	 * / * -- Dawn -- // * /
	 * JsonObject rootAlarmsDawn = rootAlarms.createNestedObject("Dawn");
	 */

	/*
	 * rootAlarmsDawn["Volume"]   = alarms.getDawnVolume().value();   / * Volume // * /
	 * rootAlarmsDawn["Time"]     = alarms.getDawnTime().value();     / * Time // * /
	 * rootAlarmsDawn["Duration"] = alarms.getDawnDuration().value(); / * Duration // * /
	 */

	/*
	 * / * -- Sunset -- // * /
	 * JsonObject rootAlarmsSunset = rootAlarms.createNestedObject("Sunset");
	 */

	/*
	 * rootAlarmsSunset["Duration"]     = alarms.getSunsetDuration().value();     / * Time // * /
	 * rootAlarmsSunset["DecreaseTime"] = alarms.getSunsetDecreaseTime().value(); / * Duration // * /
	 */

	if (!cJSON_PrintPreallocated(root, buf, size, pretty)) {
		ESP_LOGE(TAG, "Failed to print JSON");
		ret = -ENOMEM;
		goto end;
	}

end:
	cJSON_Delete(root);
	return ret;
}

static esp_err_t json_generate_resources(char *buf, size_t size, bool pretty)
{
	cJSON *root, *level_1, *level_2;
	esp_err_t ret = 0;

	root = cJSON_CreateObject();
	if (!root) {
		ret = -ENOMEM;
		goto end;
	}

	/******* Status *******/
	if (!cJSON_AddStringToObject(root, "Status", "Ok")) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	/******* Message *******/
	if (!cJSON_AddStringToObject(root, "Message", "")) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	/******* Light *******/
	level_1 = cJSON_AddObjectToObject(root, "Light");
	if (!level_1) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	/* -- ModeNames -- */
	level_2 = cJSON_AddArrayToObject(level_1, "ModeNames");
	if (!level_2) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	for (uint8_t mode; mode < LIGHT_MODE_N; mode++) {
		if (!cJSON_AddItemToArray(level_2, cJSON_CreateStringReference(light_mode_string_get(mode)))) {
			ESP_LOGE(TAG, "Failed to add item to Object/Array");
			ret = -ENOMEM;
			goto end;
		}
	}

	/* -- Colors -- */
	level_2 = cJSON_AddArrayToObject(level_1, "Colors");
	if (!level_2) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	for (int i = 0; i < ARRAY_SIZE(WebcolorList); i++) {
		if (!cJSON_AddItemToArray(level_2, cJSON_CreateIntArray(WebcolorList[i], ARRAY_SIZE(WebcolorList[i])))) {
			ESP_LOGE(TAG, "Failed to add item to Object/Array");
			ret = -ENOMEM;
			goto end;
		}
	}

	if (!cJSON_PrintPreallocated(root, buf, size, pretty)) {
		ESP_LOGE(TAG, "Failed to print JSON");
		ret = -ENOMEM;
		goto end;
	}

end:
	cJSON_Delete(root);
	return ret;
}

static esp_err_t json_generate_error(char *buf, size_t size, bool pretty, RequestError error)
{
	cJSON *root;
	esp_err_t ret = 0;

	root = cJSON_CreateObject();
	if (!root) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	/******* Status *******/
	if (!cJSON_AddStringToObject(root, "Status", "Error")) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	/******* Message *******/
	if (!cJSON_AddStringToObject(root, "Message", error.toString().c_str())) {
		ESP_LOGE(TAG, "Failed to add item to Object/Array");
		ret = -ENOMEM;
		goto end;
	}

	if (!cJSON_PrintPreallocated(root, buf, size, pretty)) {
		ESP_LOGE(TAG, "Failed to print JSON");
		ret = -ENOMEM;
		goto end;
	}

end:
	cJSON_Delete(root);
	return ret;
}

esp_err_t json_get_data_pretty(char *buf, size_t size)
{
	return json_generate_data(buf, size, true);
}

esp_err_t json_get_data(char *buf, size_t size)
{
	return json_generate_data(buf, size, false);
}

esp_err_t json_get_resources_pretty(char *buf, size_t size)
{
	return json_generate_resources(buf, size, true);
}

esp_err_t json_get_resources(char *buf, size_t size)
{
	return json_generate_resources(buf, size, false);
}

esp_err_t json_get_error_pretty(char *buf, size_t size, RequestError error)
{
	return json_generate_error(buf, size, error, true);
}

esp_err_t json_get_error(char *buf, size_t size, RequestError error)
{
	return json_generate_error(buf, size, error, false);
}
