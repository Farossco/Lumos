/**
 * @file alarm.c
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-13
 *
 * @brief
 */

#include "alarm.h"
#include "alarm_item.h"
#include <nvs_flash.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

static const char *TAG = "alarm";

#if CONFIG_ALARM_NVS_PART_CUSTOM
#define PARTITION_NAME CONFIG_ALARM_NVS_PART_CUSTOM_NAME
#else /* if CONFIG_ALARM_NVS_PART_CUSTOM */
#define PARTITION_NAME NVS_DEFAULT_PART_NAME
#endif /* if CONFIG_ALARM_NVS_PART_CUSTOM */

#define NAMESPACE_NAME CONFIG_ALARM_NVS_NAMESPACE_NAME

static nvs_handle_t alarm_nvs_handle;
static alarm_id_t alarm_id_cursor;
static SLIST_HEAD( , alarm_sub_data) sub_list;

static void alarm_sub_call(struct alarm_data *data)
{
	struct alarm_sub_data *sub_data;

	SLIST_FOREACH(sub_data, &sub_list, sle) {
		if (sub_data->type_id == data->type_id) {
			sub_data->cb(data->type_id, data->id);
		}
	}
}

static void alarm_key_get(char *buf, size_t size, alarm_id_t id)
{
	assert(snprintf(buf, size, "%ld", id) < size);
}

static esp_err_t alarm_nvs_set(struct alarm_data *data)
{
	esp_err_t err;
	char key[11]; /* Max uint32 value is 4 294 967 296 and accounting for '\0' */

	alarm_key_get(key, sizeof(key), data->id);

	ESP_LOGD(TAG, "Writing data to NVS (key: %s)", key);

	err = nvs_set_blob(alarm_nvs_handle, key, data, sizeof(*data));
	if (err) {
		ESP_LOGE(TAG, "Failed to write data to NVS: %s", esp_err_to_name(err));
		return err;
	}

	return ESP_OK;
}

static esp_err_t alarm_nvs_rm(struct alarm_data *data)
{
	esp_err_t err;
	char key[11]; /* Max uint32 value is 4 294 967 296 and accounting for '\0' */

	alarm_key_get(key, sizeof(key), data->id);

	ESP_LOGD(TAG, "Removing data from NVS (key: %s)", key);

	err = nvs_erase_key(alarm_nvs_handle, key);
	if (err) {
		ESP_LOGE(TAG, "Failed to remove entry: %s", esp_err_to_name(err));
		return err;
	}

	return ESP_OK;
}

static bool alarm_is_active_now(struct alarm_schedule *schedule, struct tm *timeinfo)
{
	return schedule->active_on.u8 && ((1 << timeinfo->tm_wday) != 0) &&
	       schedule->time.hour == timeinfo->tm_hour &&
	       schedule->time.min == timeinfo->tm_min &&
	       schedule->time.sec == timeinfo->tm_sec;
}

static void alarm_task(void *arg)
{
	struct alarm_data *data;
	struct alarm_item *item;
	TickType_t last_wake;
	time_t now;
	struct tm timeinfo;

	last_wake = xTaskGetTickCount();

	while (true) {
		time(&now);
		localtime_r(&now, &timeinfo);

		item = alarm_item_get_next(NULL);

		while (item) {
			data = alarm_item_data_ptr_get(item);

			if (data->active &&
			    alarm_is_active_now(&data->schedule, &timeinfo)) {
				ESP_LOGI(TAG, "Triggering alarm %ld", data->id);
				alarm_sub_call(data);
			}

			item = alarm_item_get_next(item);
		}

		xTaskDelayUntil(&last_wake, 1000 / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}

esp_err_t alarm_create(alarm_type_id_t type_id, struct alarm_schedule *schedule, bool persistent)
{
	esp_err_t err;
	struct alarm_item *item;
	struct alarm_data *data;

	assert(alarm_nvs_handle);

	ESP_LOGD(TAG, "Adding alarm with type %d to the list", type_id);

	item = alarm_item_create();
	if (!item) {
		ESP_LOGE(TAG, "Failed to create item!");
		return ESP_ERR_NO_MEM;
	}

	data = alarm_item_data_ptr_get(item);

	data->id         = alarm_id_cursor++;
	data->type_id    = type_id;
	data->persistent = persistent;
	data->schedule   = *schedule;
	data->active     = true;

	if (persistent) {
		err = alarm_nvs_set(data);
		if (err) {
			return err;
		}
	}

	return ESP_OK;
}

esp_err_t alarm_delete(alarm_id_t id)
{
	esp_err_t err;
	struct alarm_item *item;
	struct alarm_data *data;

	item = alarm_item_get_from_id(id);
	if (item == NULL) {
		ESP_LOGE(TAG, "Failed to find alarm with id %ld in the list", id);
		return ESP_ERR_NOT_FOUND;
	}

	data = alarm_item_data_ptr_get(item);

	ESP_LOGD(TAG, "Removing alarm %ld from the list", id);

	if (data->persistent) {
		err = alarm_nvs_rm(data);
		if (err) {
			return err;
		}
	}

	alarm_item_rm(item);

	return ESP_OK;
}

esp_err_t alarm_edit(alarm_id_t id, struct alarm_schedule *schedule)
{
	struct alarm_item *item;
	struct alarm_data *data;
	esp_err_t err;

	item = alarm_item_get_from_id(id);
	if (item == NULL) {
		ESP_LOGE(TAG, "Failed to find alarm with id %ld in the list", id);
		return ESP_ERR_NOT_FOUND;
	}

	data = alarm_item_data_ptr_get(item);

	ESP_LOGD(TAG, "Editting alarm %ld", id);

	data->schedule = *schedule;

	if (data->persistent) {
		err = alarm_nvs_set(data);
		if (err) {
			return err;
		}
	}

	return ESP_OK;
}

esp_err_t alarm_set_active(alarm_id_t id, bool active)
{
	struct alarm_item *item;
	struct alarm_data *data;
	esp_err_t err;

	item = alarm_item_get_from_id(id);
	if (item == NULL) {
		ESP_LOGE(TAG, "Failed to find alarm with id %ld in the list", id);
		return ESP_ERR_NOT_FOUND;
	}

	data = alarm_item_data_ptr_get(item);

	ESP_LOGD(TAG, "Activating alarm %ld", id);

	data->active = active;

	if (data->persistent) {
		err = alarm_nvs_set(data);
		if (err) {
			return err;
		}
	}

	return ESP_OK;
}

esp_err_t alarm_clear(void)
{
	esp_err_t err;

	alarm_item_clr();

	alarm_id_cursor = 0;

	err = nvs_erase_all(alarm_nvs_handle);
	if (err) {
		ESP_LOGE(TAG, "Failed to erase all entries from NVS");
		return err;
	}

	return ESP_OK;
}

esp_err_t alarm_iterate(alarm_iterate_cb cb, const void *user_data)
{
	esp_err_t err;
	struct alarm_item *item;
	struct alarm_data *data;

	ESP_LOGI(TAG, "Iterating through alarms");

	item = alarm_item_get_next(NULL);

	while (item != NULL) {
		data = alarm_item_data_ptr_get(item);

		err = cb(data->id, data->type_id, data->active, data->schedule, user_data);
		if (err) {
			ESP_LOGE(TAG, "Error raised by iteration callback: %s", esp_err_to_name(err));
			return err;
		}

		item = alarm_item_get_next(item);
	}

	ESP_LOGI(TAG, "Iteration done");

	return ESP_OK;
}

esp_err_t alarm_sub(struct alarm_sub_data *sub_data)
{
	assert(sub_data);
	assert(sub_data->cb);

	struct alarm_sub_data *browse_obj;

	/* Browse the list to check for duplicate */
	SLIST_FOREACH(browse_obj, &sub_list, sle) {
		if (browse_obj == sub_data) {
			ESP_LOGE(TAG, "Subscriber with type %d (%p) already in the list", sub_data->type_id, sub_data);
			return ESP_ERR_INVALID_STATE;
		}
	}

	SLIST_INSERT_HEAD(&sub_list, sub_data, sle);

	ESP_LOGD(TAG, "Successfully subscribed to alarm type %d", sub_data->type_id);

	return ESP_OK;
}

esp_err_t alarm_init(void)
{
	esp_err_t err;
	nvs_iterator_t iterator = NULL;

/* No need to initialize the default "nvs" partition */
#if CONFIG_ALARM_NVS_PART_CUSTOM
	err = nvs_flash_init_partition(PARTITION_NAME);
	if (err) {
		ESP_LOGE(TAG, "Failed to initialize '%s' partition: %s", PARTITION_NAME, esp_err_to_name(err));
		return err;
	}
#endif /* if CONFIG_SETTINGS_IMPL_NVS_PART_CUSTOM */

	err = nvs_open_from_partition(PARTITION_NAME, NAMESPACE_NAME, NVS_READWRITE, &alarm_nvs_handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to open NVS namespace '%s' in partition '%s': %s",
		         NAMESPACE_NAME, PARTITION_NAME, esp_err_to_name(err));
		return err;
	}

	ESP_LOGI(TAG, "Loading alarms from NVS");

	err = nvs_entry_find(PARTITION_NAME, NAMESPACE_NAME, NVS_TYPE_BLOB, &iterator);
	while (err == ESP_OK) {
		nvs_entry_info_t entry_info;
		struct alarm_item *item;
		struct alarm_data *data;
		size_t length;
		bool err_erase = false;

		/* Can omit error check if parameters are guaranteed to be non-NULL */
		nvs_entry_info(iterator, &entry_info);

		item = alarm_item_create();
		if (!item) {
			ESP_LOGE(TAG, "Failed to create item!");
			return ESP_ERR_NO_MEM;
		}

		data = alarm_item_data_ptr_get(item);

		length = sizeof(*data);

		err = nvs_get_blob(alarm_nvs_handle, entry_info.key, data, &length);
		if (err && err != ESP_ERR_INVALID_SIZE) {
			ESP_LOGE(TAG, "Failed to read NVS: %s", esp_err_to_name(err));
			err_erase = true;
		} else if (length != sizeof(*data)) {
			ESP_LOGE(TAG, "Found incorrect data in NVS: %s (length: %d), removing", entry_info.key, length);
			err_erase = true;
		} else if (!data->persistent) {
			ESP_LOGE(TAG, "Found non-persistent item in NVS: %s, removing", entry_info.key);
			err_erase = true;
		} else {
			ESP_LOGD(TAG, "\tLoaded alarm %ld - Type: %d - Time: %dh %dmin %dsec", data->id,
			         data->type_id, data->schedule.time.hour, data->schedule.time.min, data->schedule.time.sec);

			if (alarm_id_cursor <= data->id) {
				alarm_id_cursor = data->id + 1;
			}
		}

		if (err_erase) {
			alarm_item_rm(item);

			err = nvs_erase_key(alarm_nvs_handle, entry_info.key);
			if (err) {
				ESP_LOGE(TAG, "Failed to remove entry: %s", esp_err_to_name(err));
				return err;
			}
		}

		err = nvs_entry_next(&iterator);
	}
	if (err != ESP_ERR_NVS_NOT_FOUND) {
		ESP_LOGE(TAG, "An error happened during NVS iteration: %s", esp_err_to_name(err));
		return err;
	}

	nvs_release_iterator(iterator);

	ESP_LOGI(TAG, "Loading done, starting task");

	xTaskCreatePinnedToCore(alarm_task, "alarm_task", 4096, NULL, CONFIG_ALARM_TASK_PRIO, NULL, CONFIG_ALARM_TASK_CORE);

	return ESP_OK;
}
