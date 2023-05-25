#include "settings_impl.h"

#include <nvs_flash.h>
#include <esp_log.h>
#include <esp_err.h>

static const char *TAG = "settings_impl_nvs";

#if CONFIG_SETTINGS_IMPL_NVS_PART_CUSTOM
#define PARTITION_NAME CONFIG_SETTINGS_IMPL_NVS_PART_CUSTOM_NAME
#else /* if CONFIG_SETTINGS_IMPL_NVS_PART_CUSTOM */
#define PARTITION_NAME NVS_DEFAULT_PART_NAME
#endif /* if CONFIG_SETTINGS_IMPL_NVS_PART_CUSTOM */

esp_err_t settings_impl_write(const char *group_name, const char *key_name, void *data, size_t size)
{
	esp_err_t err;
	nvs_handle_t handle;

	err = nvs_open_from_partition(PARTITION_NAME, group_name, NVS_READWRITE, &handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to open nvs for group %s: %s", group_name, esp_err_to_name(err));
		return err;
	}

	err = nvs_set_blob(handle, key_name, data, size);
	if (err) {
		ESP_LOGE(TAG, "Could not write key %s of group %s: %s", key_name, group_name, esp_err_to_name(err));
	} else {
		nvs_close(handle);
		return err;
	}

	err = nvs_commit(handle);
	if (err) {
		ESP_LOGE(TAG, "Failed to commit changes");
	} else {
		nvs_close(handle);
		return err;
	}

	nvs_close(handle);

	return ESP_OK;
}

esp_err_t settings_impl_delete(const char *group_name, const char *key_name)
{
	esp_err_t err;
	nvs_handle_t handle;

	err = nvs_open_from_partition(PARTITION_NAME, group_name, NVS_READWRITE, &handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to open nvs for group %s: %s", group_name, esp_err_to_name(err));
		return err;
	}

	err = nvs_erase_key(handle, key_name);
	if (err && err != ESP_ERR_NVS_NOT_FOUND) {
		ESP_LOGE(TAG, "Could not erase key %s of group %s: %s", key_name, group_name, esp_err_to_name(err));
	} else {
		nvs_close(handle);
		return err;
	}

	err = nvs_commit(handle);
	if (err) {
		ESP_LOGE(TAG, "Failed to commit changes");
	} else {
		nvs_close(handle);
		return err;
	}

	nvs_close(handle);

	return ESP_OK;
}

struct settings_impl_read_func_arg {
	nvs_handle_t     handle;
	nvs_entry_info_t info;
};

static esp_err_t settings_impl_read(void *data, size_t size, struct settings_impl_read_func_arg *arg)
{
	esp_err_t err;
	size_t blob_size;

	/* Partition is already opened when calling this callback */

	/* Get the size of the data */
	err = nvs_get_blob(arg->handle, arg->info.key, NULL, &blob_size);
	if (err) {
		ESP_LOGE(TAG, "Could not get data size of key %s: %s", arg->info.key, esp_err_to_name(err));
		return err;
	}

	if (size != blob_size) {
		return ESP_ERR_INVALID_SIZE;
	}

	/* Read the data */
	err = nvs_get_blob(arg->handle, arg->info.key, data, &blob_size);
	if (err) {
		ESP_LOGE(TAG, "Could not read key %s: %s", arg->info.key, esp_err_to_name(err));
		return err;
	}

	return ESP_OK;
}

esp_err_t settings_impl_load(const char *group_name, settings_impl_load_cb_t cb, void *user_data)
{
	esp_err_t err;
	nvs_iterator_t iterator = NULL;
	struct settings_impl_read_func_arg arg;

	/* Callback cannot be NULL */
	assert(cb);

	err = nvs_open_from_partition(PARTITION_NAME, group_name, NVS_READWRITE, &arg.handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to open nvs for group %s: %s", group_name, esp_err_to_name(err));
		return err;
	}

	err = nvs_entry_find(PARTITION_NAME, group_name, NVS_TYPE_BLOB, &iterator);
	while (err == ESP_OK) {
		/* Can omit error check if parameters are guaranteed to be non-NULL */
		nvs_entry_info(iterator, &arg.info);

		err = cb(arg.info.key, settings_impl_read, &arg, user_data);
		if (err) {
			ESP_LOGE(TAG, "Error raised by read_cb: %s", esp_err_to_name(err));
			nvs_release_iterator(iterator);
			nvs_close(arg.handle);
			return err;
		}

		err = nvs_entry_next(&iterator);
	}
	nvs_release_iterator(iterator);

	nvs_close(arg.handle);

	return ESP_OK;
}

esp_err_t settings_impl_init(void)
{
	esp_err_t err;

	esp_log_level_set(TAG, ESP_LOG_VERBOSE);

	ESP_LOGI(TAG, "Initializing implementation");

/* No need to initialize the default "nvs" partition */
#if CONFIG_SETTINGS_IMPL_NVS_PART_CUSTOM
	err = nvs_flash_init_partition(PARTITION_NAME);
	if (err) {
		ESP_LOGE(TAG, "Failed to initialize %s partition: %s", PARTITION_NAME, esp_err_to_name(err));
		return err;
	}
#endif /* if CONFIG_SETTINGS_IMPL_NVS_PART_CUSTOM */

	return ESP_OK;
}
