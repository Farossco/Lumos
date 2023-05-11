/**
 * @file settings.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-07
 *
 * @brief
 */

#pragma once

#include <settings_def.h>
#include <esp_err.h>
#include <stddef.h>

/**
 * @brief Helper macro for the settings_set() function.
 *
 *        This allows to abstract the size argument and to shorten the id macro
 *        (ex: use PROFILE intead of SETTINGS_PROFILE).
 *
 */
#define SETTINGS_SET(key_id, value) settings_set(SETTINGS_ ## key_id, &value, sizeof(value))

/**
 * @brief Helper macro for the settings_get() function.
 *
 *        This allows to abstract the size argument and to shorten the id macro
 *        (ex: use PROFILE intead of SETTINGS_PROFILE).
 *
 */
#define SETTINGS_GET(key_id, value) settings_get(SETTINGS_ ## key_id, &value, sizeof(value))

/**
 * @brief Helper macro for the settings_reset() function.
 *
 *        This allows to abstract the size argument and to shorten the id macro
 *        (ex: use PROFILE intead of SETTINGS_PROFILE).
 *
 */
#define SETTINGS_RESET(key_id, value) settings_reset(SETTINGS_ ## key_id, &value, sizeof(value))

/**
 * @brief Helper macro for the settings_reset() function with no output needed.
 *
 *        This allows to abstract the buffer and size arguments and to shorten the id macro
 *        (ex: use PROFILE intead of SETTINGS_PROFILE).
 *
 */
#define SETTINGS_RESET_NO_OUTPUT(key_id) settings_reset(SETTINGS_ ## key_id, NULL, 0)

/**
 * @brief Helper macro for the settings_group_reset() function.
 *
 *        This allows to shorten the id macro
 *        (ex: use PROFILE intead of SETTINGS_PROFILE).
 *
 */
#define SETTINGS_GROUP_RESET(group_id) settings_group_reset(SETTINGS_GROUP_ ## group_id)

/* Id types */
typedef uint16_t settings_key_id_t;
typedef uint16_t settings_group_id_t;

/**
 * @brief Settings version structure.
 *
 */
struct settings_version {
	uint8_t major;
	uint8_t minor;
	uint8_t rev;
};

/**
 * @brief Writes the given setting in non-volatile memory.
 *
 *        Prefer using the SETTINGS_RESET or SETTINGS_RESET_NO_OUTPUT macro
 *        instead to abstract the size argument and simplify the key id
 *        (ex: PROFILE intead of SETTINGS_PROFILE).
 *
 *        Caution! The size of data must match the size of the setting.
 *        Otherwise, an error will be thrown and the data won't be written.
 *
 * @param key_id The setting key.
 * @param data   The buffer containing the data to set.
 * @param size   The size of the buffer.
 *
 * @return ESP_OK if success.
 *         ESP_ERR_INVALID_ARG if the id is invalid or if a deprecated key is used.
 *         ESP_ERR_INVALID_SIZE if the provided size doesn't match the size of the setting.
 *         ESP_ERR_INVALID_STATE if the library has not been initialized.
 *         ESP_ERR_TIMEOUT if another action is in progress and the timeout has been reached.
 *         Any other error code returned by the underlying non-volatile implementation.
 */
esp_err_t settings_set(settings_key_id_t key_id, const void *data, size_t size);

/**
 * @brief Reads the given setting.
 *
 *        Prefer using the SETTINGS_RESET or SETTINGS_RESET_NO_OUTPUT macro
 *        instead to abstract the size argument and simplify the key id
 *        (ex: PROFILE intead of SETTINGS_PROFILE).
 *
 *        Caution! The size of data must match the size of the setting.
 *        Otherwise, an error will be thrown and the data won't be read.
 *
 * @param key_id The setting key.
 * @param data   The buffer to store the data to get.
 * @param size   The size of the buffer.
 *
 * @return ESP_OK if success.
 *         ESP_ERR_INVALID_ARG if the id is invalid or if a deprecated key is used.
 *         ESP_ERR_INVALID_SIZE if the provided size doesn't match the size of the setting.
 *         ESP_ERR_INVALID_STATE if the library has not been initialized.
 *         ESP_ERR_TIMEOUT if another action is in progress and the timeout has been reached.
 *         Any other error code returned by the underlying non-volatile implementation.
 */
esp_err_t settings_get(settings_key_id_t key_id, void *data, size_t size);

/**
 * @brief Reset the setting to its default value and outputs the new value.
 *
 *        Prefer using the SETTINGS_RESET or SETTINGS_RESET_NO_OUTPUT macro
 *        instead to abstract the size argument and simplify the key id
 *        (ex: PROFILE intead of SETTINGS_PROFILE).
 *
 *        Caution! If provided, the size of data must match the size of the
 *        setting. Otherwise, an error will be thrown and data won't be output.
 *        The data will still be reset anyway.
 *
 * @param key_id The setting key.
 * @param data   The buffer to store the new reset data. NULL for no output.
 * @param size   The size of the buffer.
 *
 * @return ESP_OK if success.
 *         ESP_ERR_INVALID_ARG if the id is invalid or if a deprecated key is used.
 *         ESP_ERR_INVALID_SIZE if the provided size doesn't match the size of the setting
 *                              (only if a data buffer is provided).
 *         ESP_ERR_INVALID_STATE if the library has not been initialized.
 *         ESP_ERR_TIMEOUT if another action is in progress and the timeout has been reached.
 *         Any other error code returned by the underlying non-volatile implementation.
 */
esp_err_t settings_reset(settings_key_id_t key_id, void *data, size_t size);

/**
 * @brief Clear all value of the loaded instance of a group designated by its
 *        selector and reloads them.
 *
 *        When calling this function, the current instance of the group will remain but
 *        its values will be reset. The group is designated by SETTINGS_GROUP_XXX.
 *
 * @param group_id The group id.
 *
 * @return ESP_OK if success.
 *         ESP_ERR_INVALID_ARG if the id is invalid.
 *         ESP_ERR_INVALID_STATE if the library has not been initialized.
 *         ESP_ERR_TIMEOUT if another action is in progress and the timeout has been reached.
 *         Any other error code returned by the underlying non-volatile implementation.
 */
esp_err_t settings_group_reset(settings_group_id_t group_id);

/**
 * @brief Get the current version of the settings.
 *
 * This value is defined at compile time and cannot be changed.
 *
 * @return The current version.
 */
struct settings_version settings_version_get(void);

/**
 * @brief Reload all settings from persistent memory.
 *
 * This function must be called after a multi-group selector value has been
 * changed
 *
 * @return ESP_OK if success.
 *         ESP_ERR_INVALID_STATE if the library has not been initialized.
 *         ESP_ERR_TIMEOUT if another action is in progress and the timeout has been reached.
 *         Any other error code returned by the underlying non-volatile implementation.
 */
esp_err_t settings_reload(void);

/**
 * @brief Initializes the settings library.
 *
 * This is necessary to initialize the non-volatile implementation and load
 * all the settings values.
 *
 * To reload data (after having changed a multi-group selector for instance),
 * use itl_settings_reload().
 *
 * @return ESP_OK if success.
 *         ESP_ERR_INVALID_STATE if the library has already been initialized.
 *         ESP_ERR_NO_MEM if the library failed to allocate the necessary memory.
 *         Any other error code returned by the underlying non-volatile implementation.
 */
esp_err_t settings_init(void);
