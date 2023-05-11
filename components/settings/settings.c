/**
 * @file settings.c
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-07
 *
 * @brief
 */

#include "settings.h"
#include "settings_impl.h"
#include "settings_internal.h"
#include "settings_def_internal.h"

#include <stdio.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <string.h>

#if CONFIG_SETTINGS_BLOCKING
#define BLOCKING_TIMEOUT_MS CONFIG_SETTINGS_BLOCKING_TIMEOUT_MS
#else /* if CONFIG_SETTINGS_BLOCKING */
#define BLOCKING_TIMEOUT_MS 0
#endif /* if CONFIG_SETTINGS_BLOCKING */

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

const char *TAG = "settings";

struct settings_group_item {
	const struct settings_key_item *const selector_item;
	const char *const                     name;
	char *const                           full_name_ptr;
};

struct settings_key_item {
	const struct settings_group_item *const group_item;
	const char *const                       name;
	const ptrdiff_t                         data_offset;
	const size_t                            data_size;
};

/**
 * @brief Auto-generated data from input csv
 * 
 */
struct settings_data {
	SETTINGS_DATA
};

/**
 * @brief Auto-generated group full-name buffers from input csv
 * 
 */
struct settings_groups_full_names {
	SETTINGS_GROUP_FULL_NAMES
};

SemaphoreHandle_t settings_mutex;

static bool settings_initialized;

static struct settings_data settings_data;
static const struct settings_data settings_data_default = SETTINGS_DATA_DEFAULTS;
static struct settings_groups_full_names __unused settings_groups_full_names;

/* Pre-definition to be used in SETTINGS_GROUPS */
static const struct settings_key_item settings_keys[];

static const struct settings_group_item settings_groups[] = { SETTINGS_GROUPS };
static const struct settings_key_item settings_keys[]     = { SETTINGS_KEYS };

static const struct settings_version version = SETTINGS_CURRENT_VERSION;

/**
 * @brief Get the name of a given group item.
 *
 *        This function will return the full name if the given group
 *        is a multi-instance group. (example: profile3)
 *
 * @param group_item The group item
 *
 * @return The name of the group
 */
static const char * group_name_get(const struct settings_group_item *group_item)
{
	/* Checks weather the group can have multiple instances */
	if (group_item->selector_item != NULL) {
		return group_item->full_name_ptr;
	} else {
		return group_item->name;
	}
}

/**
 * @brief Get a group item from its id.
 *
 *        Caution: No verification is done on the group id (expect assert)
 *        so proper checking must be done before calling this function.
 *
 * @param group_id The group id
 *
 * @return The group item.
 */
static const struct settings_group_item * group_item_get(settings_group_id_t group_id)
{
	assert(group_id < ARRAY_SIZE(settings_groups));

	return &settings_groups[group_id];
}

/**
 * @brief Gets a pointer at the start of the data for a given item
 *
 * @param key_item The key item
 *
 * @return Data pointer
 */
static inline void * key_data_ptr_get(const struct settings_key_item *key_item)
{
	assert(key_item);

	return ((uint8_t *)&settings_data) + key_item->data_offset;
}

/**
 * @brief Gets a pointer at the start of the default data for a given item
 *
 * @param key_item The key item
 *
 * @return Data pointer
 */
static inline const void * key_data_ptr_get_default(const struct settings_key_item *key_item)
{
	return ((uint8_t *)&settings_data_default) + key_item->data_offset;
}

/**
 * @brief Copies the data for a given key item from volatile memory to the provided buffer.
 *
 * @param key_item The key item
 * @param data     The data buffer
 * @param size     The size of the buffer
 *
 * @return ESP_ERR_INVALID_SIZE if the provided size doesn't match the size of the item data
 */
static esp_err_t data_get(const struct settings_key_item *key_item, void *data, size_t size)
{
	if (size != key_item->data_size) {
		ESP_LOGE(TAG, "Incorrect size (%d should be %d)", size, key_item->data_size);
		return ESP_ERR_INVALID_SIZE;
	}

	memcpy(data, key_data_ptr_get(key_item), key_item->data_size);

	return ESP_OK;
}

/**
 * @brief Copies the provided data for a given key item to the volatile memory.
 *
 *        Caution: This function does not write the data to persistent memory.
 *
 * @param key_item The key item
 * @param data     The data buffer
 * @param size     The size of the buffer
 *
 * @return ESP_ERR_INVALID_SIZE if the provided size doesn't match the size of the item data
 */
static esp_err_t data_set(const struct settings_key_item *key_item, const void *data, size_t size)
{
	if (size != key_item->data_size) {
		ESP_LOGE(TAG, "Incorrect size (%d should be %d)", size, key_item->data_size);
		return ESP_ERR_INVALID_SIZE;
	}

	memcpy(key_data_ptr_get(key_item), data, key_item->data_size);

	return ESP_OK;
}

/**
 * @brief Resets the data in volatile memory for a given key item to its default value.
 *
 *        Caution: This function does not write the persistent memory.
 *
 * @param key_item The key item
 */
static void data_reset(const struct settings_key_item *key_item)
{
	memcpy(key_data_ptr_get(key_item), key_data_ptr_get_default(key_item), key_item->data_size);
}

/**
 * @brief Save data to persistent memory
 *
 *        Keys in multi-group of instance 0 are not saved to persistent memory
 *
 * @param key_item The key item
 *
 * @return ESP_OK on success.
 *         Error code dependant of the implementation otherwise.
 */
static esp_err_t data_save(const struct settings_key_item *key_item)
{
	/* Do not write to persistent memory for group 0 */
	if (key_item->group_item->full_name_ptr &&
	    key_item->group_item->full_name_ptr[0] == '\0') {
		return ESP_OK;
	}

	return settings_impl_write(group_name_get(key_item->group_item), key_item->name,
	                           key_data_ptr_get(key_item), key_item->data_size);
}

/**
 * @brief Delete data from persistent memory
 *
 * @param key_item The key item
 *
 * @return ESP_OK on success.
 *         Error code dependant of the implementation otherwise.
 */
static esp_err_t data_delete(const struct settings_key_item *key_item)
{
	return settings_impl_delete(group_name_get(key_item->group_item), key_item->name);
}

/**
 * @brief Get a key item from its id.
 *
 *        Caution: No verification is done on the key id (expect assert)
 *        so proper checking must be done before calling this function.
 *
 * @param key_id The key id
 *
 * @return The key item.
 */
static const struct settings_key_item * key_item_get(settings_key_id_t key_id)
{
	assert(key_id < ARRAY_SIZE(settings_keys));

	return &settings_keys[key_id];
}

/**
 * @brief Get a key item from its name.
 *
 * @param key_name The key name
 *
 * @return The key item
 */
static const struct settings_key_item * key_item_get_from_name(const char *key_name)
{
	assert(key_name);

	for (esp_err_t i = 0; i < ARRAY_SIZE(settings_keys); i++) {
		/* Skip deprecated keys */
		if (settings_keys[i].group_item == NULL) {
			continue;
		}

		if (strcmp(key_name, settings_keys[i].name) == 0) {
			return &settings_keys[i];
		}
	}

	return NULL;
}

/**
 * @brief Parameters to be passed on to on_key_item_loaded() function
 */
struct on_key_item_loaded_params {
	const struct settings_group_item *group_item;
};

/**
 * @brief Callback function.
 *
 *        This function is called for each key of a given group found in persistent memory
 *        and receives the name of the key that has been found.
 *
 *        Note: A return value different that ESP_OK will stop the search in persistent memory.
 *        When returning an error code, this error code will be passed to the call of
 *        settings_impl_load().
 *
 * @param key_name      Name of the key
 * @param read_func     Read function. Call this function to read the data from persistent memory
 * @param read_func_arg Argument to be passed along to the function
 * @param user_data     User data provided when calling settings_impl_load()
 *
 * @return ESP_OK if everything went well.
 *         Error code from implementation or read_func otherwise.
 */
static esp_err_t on_key_item_loaded(const char *key_name, settings_impl_read_func read_func,
                                    struct settings_impl_read_func_arg *read_func_arg, void *user_data)
{
	struct on_key_item_loaded_params *params = user_data;
	const struct settings_key_item *key_item;
	esp_err_t err;

	ESP_LOGD(TAG, "Loading key \"%s/%s\"", params->group_item->name, key_name);

	key_item = key_item_get_from_name(key_name);
	if (key_item != NULL) {
		err = read_func(key_data_ptr_get(key_item), key_item->data_size, read_func_arg);
		if (err == ESP_ERR_INVALID_SIZE) {
			ESP_LOGW(TAG, "Loaded key %s has incorrect size", key_name);
			/* Continue and delete the data from flash */
		} else if (err) {
			ESP_LOGE(TAG, "read_cb error: %s", esp_err_to_name(err));
			return err;
		} else {
			return ESP_OK;
		}
	}

	ESP_LOGD(TAG, "Unknown entry, deleting it");

	err = settings_impl_delete(params->group_item->name, key_name);
	if (err) {
		ESP_LOGE(TAG, "Error while deleting entry: %s", esp_err_to_name(err));
		return err;
	}

	return ESP_OK;
}

esp_err_t settings_set(settings_key_id_t key_id, const void *data, size_t size)
{
	esp_err_t err;
	const struct settings_key_item *key_item;

	assert(data);

	if (key_id >= ARRAY_SIZE(settings_keys)) {
		ESP_LOGE(TAG, "Incorrect key: %d", key_id);
		return ESP_ERR_INVALID_ARG;
	}

	if (settings_keys[key_id].name == NULL) {
		ESP_LOGW(TAG, "Trying to use a deprecated key is not allowed");
		return ESP_ERR_INVALID_ARG;
	}

	if (!settings_initialized) {
		ESP_LOGE(TAG, "Settings library not initialized");
		return ESP_ERR_INVALID_STATE;
	}

	if (xSemaphoreTake(settings_mutex, pdMS_TO_TICKS(BLOCKING_TIMEOUT_MS)) == pdFALSE) {
		return ESP_ERR_TIMEOUT;
	}

	key_item = key_item_get(key_id);

	err = data_set(key_item, data, size);
	if (err) {
		assert(xSemaphoreGive(settings_mutex));
		return err;
	}

	err = data_save(key_item);
	if (err) {
		assert(xSemaphoreGive(settings_mutex));
		return err;
	}

	assert(xSemaphoreGive(settings_mutex));

	return ESP_OK;
}

esp_err_t settings_get(settings_key_id_t key_id, void *data, size_t size)
{
	esp_err_t err;
	const struct settings_key_item *key_item;

	assert(data);

	if (key_id >= ARRAY_SIZE(settings_keys)) {
		ESP_LOGE(TAG, "Incorrect key: %d", key_id);
		return ESP_ERR_INVALID_ARG;
	}

	if (settings_keys[key_id].name == NULL) {
		ESP_LOGW(TAG, "Trying to use a deprecated key is not allowed");
		return ESP_ERR_INVALID_ARG;
	}

	if (!settings_initialized) {
		ESP_LOGE(TAG, "Settings library not initialized");
		return ESP_ERR_INVALID_STATE;
	}

	if (xSemaphoreTake(settings_mutex, pdMS_TO_TICKS(BLOCKING_TIMEOUT_MS)) == pdFALSE) {
		return ESP_ERR_TIMEOUT;
	}

	key_item = key_item_get(key_id);

	err = data_get(key_item, data, size);
	if (err) {
		assert(xSemaphoreGive(settings_mutex));
		return err;
	}

	assert(xSemaphoreGive(settings_mutex));

	return ESP_OK;
}

esp_err_t settings_reset(settings_key_id_t key_id, void *data, size_t size)
{
	esp_err_t err;
	const struct settings_key_item *key_item;

	if (key_id >= ARRAY_SIZE(settings_keys)) {
		ESP_LOGE(TAG, "Incorrect key: %d", key_id);
		return ESP_ERR_INVALID_ARG;
	}

	if (settings_keys[key_id].name == NULL) {
		ESP_LOGW(TAG, "Trying to use a deprecated key is not allowed");
		return ESP_ERR_INVALID_ARG;
	}

	if (!settings_initialized) {
		ESP_LOGE(TAG, "Settings library not initialized");
		return ESP_ERR_INVALID_STATE;
	}

	if (xSemaphoreTake(settings_mutex, pdMS_TO_TICKS(BLOCKING_TIMEOUT_MS)) == pdFALSE) {
		return ESP_ERR_TIMEOUT;
	}

	key_item = key_item_get(key_id);

	/* Delete the entry from persistent memory */
	err = data_delete(key_item);
	if (err) {
		assert(xSemaphoreGive(settings_mutex));
		return err;
	}

	/* Load default value */
	data_reset(key_item);

	/* Output reset value */
	if (data) {
		err = data_get(key_item, data, size);
		if (err) {
			assert(xSemaphoreGive(settings_mutex));
			return err;
		}
	}

	assert(xSemaphoreGive(settings_mutex));

	return ESP_OK;
}

esp_err_t settings_group_reset(settings_group_id_t group_id)
{
	esp_err_t err;
	const struct settings_group_item *group_item;

	if (group_id >= ARRAY_SIZE(settings_groups)) {
		ESP_LOGE(TAG, "Incorrect group id: %d", group_id);
		return ESP_ERR_INVALID_ARG;
	}

	if (!settings_initialized) {
		ESP_LOGE(TAG, "Settings library not initialized");
		return ESP_ERR_INVALID_STATE;
	}

	if (xSemaphoreTake(settings_mutex, pdMS_TO_TICKS(BLOCKING_TIMEOUT_MS)) == pdFALSE) {
		return ESP_ERR_TIMEOUT;
	}

	group_item = group_item_get(group_id);

	/* Reset each value of this group */
	for (uint16_t i = 0; i < ARRAY_SIZE(settings_keys); i++) {
		const struct settings_key_item *key_item = &settings_keys[i];

		if (key_item->group_item == group_item) {
			/* Delete the item from persistent memory */
			err = data_delete(key_item);
			if (err) {
				assert(xSemaphoreGive(settings_mutex));
				return err;
			}

			/* Reset volatile copy of the item */
			data_reset(key_item);
		}
	}

	assert(xSemaphoreGive(settings_mutex));

	return ESP_OK;
}

struct settings_version settings_version_get(void)
{
	return version;
}

static inline esp_err_t group_load_single(const struct settings_group_item *group_item)
{
	/* Generation error, group has no path */
	assert(group_item->name);

	return settings_impl_load(group_item->name, on_key_item_loaded, &(struct on_key_item_loaded_params) {
		.group_item = group_item
	});
}

static inline esp_err_t group_load_multi(const struct settings_group_item *group_item)
{
	esp_err_t err;
	settings_group_id_t selector_data;

	/* Generation error, group has no name */
	assert(group_item->name);

	/* Generation error, group has NULL full name pointer */
	assert(group_item->full_name_ptr);

	/* Generation error, invalid selector size */
	assert(group_item->selector_item->data_size == sizeof(settings_group_id_t));

	/* No error check as it has been done by the assert above */
	data_get(group_item->selector_item, &selector_data, sizeof(selector_data));

	if (selector_data > CONFIG_SETTINGS_GROUP_INSTANCE_MAX) {
		ESP_LOGW(TAG, "Selector can't be bigger than %d, resetting it", CONFIG_SETTINGS_GROUP_INSTANCE_MAX);

		selector_data = 0;

		/* No error check as it has been done by the assert above */
		data_set(group_item->selector_item, &selector_data, sizeof(selector_data));

		err = data_save(group_item->selector_item);
		if (err) {
			return err;
		}
	}

	/* For instance 0 (default), the keys are kept to their default values */
	if (selector_data == 0) {
		/* Write an empty string as the profile path */
		group_item->full_name_ptr[0] = '\0';

		return ESP_OK;
	}

	sprintf(group_item->full_name_ptr, "%s%d", group_item->name, selector_data);

	return settings_impl_load(group_item->full_name_ptr, on_key_item_loaded, &(struct on_key_item_loaded_params) {
		.group_item = group_item
	});
}

esp_err_t settings_reload(void)
{
	if (!settings_initialized) {
		ESP_LOGE(TAG, "Settings library not initialized");
		return ESP_ERR_INVALID_STATE;
	}

	if (xSemaphoreTake(settings_mutex, pdMS_TO_TICKS(BLOCKING_TIMEOUT_MS)) == pdFALSE) {
		return ESP_ERR_TIMEOUT;
	}

	/* Load default values */
	memcpy(&settings_data, &settings_data_default, sizeof(settings_data));

	for (uint16_t group_id = 0; group_id < ARRAY_SIZE(settings_groups); group_id++) {
		esp_err_t err;
		const struct settings_group_item *group_item = group_item_get(group_id);

		ESP_LOGD(TAG, "Loading group \"%s\"", group_item->name);

		if (group_item->selector_item != NULL) {
			err = group_load_multi(group_item);
		} else {
			err = group_load_single(group_item);
		}
		if (err) {
			assert(xSemaphoreGive(settings_mutex));
			return err;
		}
	}

	assert(xSemaphoreGive(settings_mutex));

	return ESP_OK;
}

esp_err_t settings_init(void)
{
	esp_err_t err;

	esp_log_level_set(TAG, ESP_LOG_VERBOSE);

	ESP_LOGI(TAG, "Initializing settings");

	if (settings_initialized) {
		return ESP_ERR_INVALID_STATE;
	}

	settings_mutex = xSemaphoreCreateMutex();
	if (settings_mutex == NULL) {
		ESP_LOGE(TAG, "Failed to initialize mutex");
		return ESP_ERR_NO_MEM;
	}

	err = settings_impl_init();
	if (err) {
		ESP_LOGE(TAG, "Error while initializing implementation: %s", esp_err_to_name(err));
		return err;
	}

	settings_initialized = true;

	err = settings_reload();
	if (err) {
		ESP_LOGE(TAG, "Error while loading settings for the first time: %s", esp_err_to_name(err));
		return err;
	}

	return ESP_OK;
}
