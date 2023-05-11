/**
 * @file settings_impl.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-07
 *
 * @brief
 */

#pragma once

#include "esp_err.h"

struct settings_impl_read_func_arg;

/**
 * @brief Read data in persistent memory.
 * 
 *        This function is not publicly available as it must only be called in the context of a call to
 *        settings_impl_load() by passing along the func_arg.
 * 
 *        This function reads data from persistent memory and writes it to the data buffer if the size is
 *        an exact match.
 * 
 * @param data Data buffer
 * @param size Size of the data buffer
 * @param arg Argument provided to the cb function
 *
 * @return esp_err_t ESP_OK on success.
 *                   ESP_ERR_INVALID_SIZE if the size of the buffer does not match the size of the data
 *                   in persistent memory.
 *                   Error code dependant of the implementation otherwise.
 */
typedef esp_err_t (*settings_impl_read_func)(void *data, size_t size, struct settings_impl_read_func_arg *arg);

/**
 * @brief Callback function.
 *
 *        When calling settings_impl_load(), a function of this type should be passed as argument.
 *        This function will be called for each key found in persistent memory.
 *
 *        This function will receive the name of the key that has been found.
 *        It will also receive a function to call to read the given data from memory as well as
 *        an argument to be passed along to that function.
 *
 *        The user data is also passed to this function.
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
 *         Any error code otherwise.
 */
typedef esp_err_t (*settings_impl_load_cb_t)(const char *key_name, settings_impl_read_func read_func,
                                             struct settings_impl_read_func_arg *read_func_arg, void *user_data);

/**
 * @brief Write data for a given key and group name to persistent memory.
 *
 * @param group_name The name of the group that the key belongs to
 * @param key_name   The name of the key
 * @param data       The data to write
 * @param size       The size of the data
 *
 * @return ESP_OK on success.
 *         Error code dependant of the implementation otherwise.
 */
esp_err_t settings_impl_write(const char *group_name, const char *key_name, void *data, size_t size);

/**
 * @brief Delete data for a givenkey and group from persistent memory.
 *
 * @param group_name The name of the group that the key belongs to
 * @param key_name   The name of the key
 *
 * @return ESP_OK on success.
 *         Error code dependant of the implementation otherwise.
 */
esp_err_t settings_impl_delete(const char *group_name, const char *key_name);

/**
 * @brief Load the content on persistent memory for a given group.
 *
 *        For each item found in persistent memory the cb function will be called.
 *
 * @param group_name The name of the group to look for
 * @param cb         The cb function to be called for each item
 * @param user_data  User data to be passed to the sb function
 *
 * @return ESP_OK on success.
 *         Error code dependant of the implementation otherwise.
 */
esp_err_t settings_impl_load(const char *group_name, settings_impl_load_cb_t cb, void *user_data);

/**
 * @brief Initialize the persistent memory implementation.
 *
 * @return ESP_OK on success.
 *         Error code dependant of the implementation otherwise.
 */
esp_err_t settings_impl_init(void);
