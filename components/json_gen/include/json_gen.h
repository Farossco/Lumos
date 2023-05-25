/**
 * @file json_gen.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-23
 *
 * @brief
 */

#pragma once

#include <esp_err.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/queue.h>
#include <cJSON.h>

#define JSON_GEN_CALL_AND_TEST(func_call) do { \
			esp_err_t err = func_call;         \
			if (err) {                         \
				return err;                    \
			}                                  \
} while (0)

/* JSON creation helpers */
#define JSON_GEN_ADD_BOOL(ctx, name, value)               JSON_GEN_CALL_AND_TEST(json_gen_add_bool(ctx, name, value))
#define JSON_GEN_ADD_BOOL_ARRAY(ctx, name, array, size)   JSON_GEN_CALL_AND_TEST(json_gen_add_bool_array(ctx, name, array, size))
#define JSON_GEN_ADD_INT(ctx, name, value)                JSON_GEN_CALL_AND_TEST(json_gen_add_int(ctx, name, value))
#define JSON_GEN_ADD_INT_ARRAY(ctx, name, array, size)    JSON_GEN_CALL_AND_TEST(json_gen_add_int_array(ctx, name, array, size))
#define JSON_GEN_ADD_DOUBLE(ctx, name, value)             JSON_GEN_CALL_AND_TEST(json_gen_add_double(ctx, name, value))
#define JSON_GEN_ADD_DOUBLE_ARRAY(ctx, name, array, size) JSON_GEN_CALL_AND_TEST(json_gen_add_double_array(ctx, name, array, size))
#define JSON_GEN_ADD_STRING(ctx, name, value)             JSON_GEN_CALL_AND_TEST(json_gen_add_string(ctx, name, value))
#define JSON_GEN_ADD_STRING_ARRAY(ctx, name, array, size) JSON_GEN_CALL_AND_TEST(json_gen_add_string_array(ctx, name, array, size))
#define JSON_GEN_ADD_ARRAY(ctx, name, array_ctx)          JSON_GEN_CALL_AND_TEST(json_gen_add_array(ctx, name, array_ctx))
#define JSON_GEN_ADD_OBJECT(ctx, name, object_ctx)        JSON_GEN_CALL_AND_TEST(json_gen_add_object(ctx, name, object_ctx))

/* Array creation helpers */
#define JSON_GEN_ARRAY_ADD_BOOL(ctx, value)               JSON_GEN_ADD_BOOL(ctx, "", value)
#define JSON_GEN_ARRAY_ADD_BOOL_ARRAY(ctx, array, size)   JSON_GEN_ADD_BOOL_ARRAY(ctx, "", array, size)
#define JSON_GEN_ARRAY_ADD_INT(ctx, value)                JSON_GEN_ADD_INT(ctx, "", value)
#define JSON_GEN_ARRAY_ADD_INT_ARRAY(ctx, array, size)    JSON_GEN_ADD_INT_ARRAY(ctx, "", array, size)
#define JSON_GEN_ARRAY_ADD_DOUBLE(ctx, value)             JSON_GEN_ADD_DOUBLE(ctx, "", value)
#define JSON_GEN_ARRAY_ADD_DOUBLE_ARRAY(ctx, array, size) JSON_GEN_ADD_DOUBLE_ARRAY(ctx, "", array, size)
#define JSON_GEN_ARRAY_ADD_STRING(ctx, value)             JSON_GEN_ADD_STRING(ctx, "", value)
#define JSON_GEN_ARRAY_ADD_STRING_ARRAY(ctx, array, size) JSON_GEN_ADD_STRING_ARRAY(ctx, "", array, size)
#define JSON_GEN_ARRAY_ADD_ARRAY(ctx, array_ctx)          JSON_GEN_ADD_ARRAY(ctx, "", array_ctx)
#define JSON_GEN_ARRAY_ADD_OBJECT(ctx, object_ctx)        JSON_GEN_ADD_OBJECT(ctx, "", object_ctx)

typedef uint8_t json_gen_type_id_t;

typedef struct {
	cJSON                    *root;
	struct json_gen_sub_data *sub_data;
} json_gen_cb_ctx_t;

typedef esp_err_t (*json_gen_cb_t)(const json_gen_cb_ctx_t *ctx, const void *user_data);

struct json_gen_sub_data {
	json_gen_type_id_t type_id;
	const char         *name;
	json_gen_cb_t      cb;
	const void         *user_data;

	SLIST_ENTRY(json_gen_sub_data) sle;
};

/**
 * @brief Generate a json document for the provided type id.
 *        This function will call all registered callbacks for the provided
 *        type id.
 *
 *        The generated JSON will have the following structure :
 *        {
 *            "Status":      "OK",
 *            "Message":     "",
 *            ...,
 *            "Subscriber2": {
 *                Content of subscriber 2
 *            },
 *            "Subscriber1": {
 *                Content of subscriber 1
 *            }
 *        }
 *
 *        Caution: This will dynamically allocate memory for the json document.
 *        The output buffer must be freed after it has been used! (free(output)).
 *
 *        If an error was encountered during the generation, an error code
 *        will be returned and the output will be filled with a JSON
 *        document containing the error by calling json_gen_get_error().
 *
 *        If the error json could not be created, the ouput pointer will
 *        be set to NULL.
 *
 * @param output  The address of the output pointer (must be freed)
 * @param type_id The type id
 * @param format  Whether the json should be formatted
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_NO_MEM if some memory could not be allocated.
 */
esp_err_t json_gen_get(char **output, json_gen_type_id_t type_id, bool format);

/**
 * @brief Generate an error json document containing the provided
 *        error message.
 *
 *        The generated JSON will have the following structure :
 *        {
 *            "Status":      "Error",
 *            "Message":     "[Error message in error_str]"
 *        }
 *
 *        Caution: This will dynamically allocate memory for the json document.
 *        The output buffer must be freed after it has been used! (free(output)).
 *
 *        If the error json could not be created, the ouput pointer will
 *        be set to NULL.
 *
 * @param output    The address of the output pointer (must be freed)
 * @param error_str An error string
 * @param format    Whether the json should be formatted
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_NO_MEM if some memory could not be allocated.
 */
esp_err_t json_gen_get_error(char **output, const char *error_str, bool format);

/**
 * @brief Add a subscriber to a JSON document.
 *
 *        During the generation, if the type id corresponds to this suscriber,
 *        the specified callback will be called to generate an object with the
 *        specified name.
 *
 * @param  sub_data The subscriber data
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_INVALID_STATE if the sub_data objet is already in the list.
 */
esp_err_t json_gen_sub(struct json_gen_sub_data *sub_data);


/* ----- The following function shall be called inside a generation callback -----
 * The callback context from the callback must be provided */

/**
 * @brief -- Shall be called inside generation callback --
 *
 *        Add a boolean value.
 *
 * @param ctx   The cb context object
 * @param name  The name of the field
 * @param value The boolean value
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_NO_MEM if some memory could not be allocated.
 */
esp_err_t json_gen_add_bool(const json_gen_cb_ctx_t *ctx, const char *const name, bool value);

/**
 * @brief -- Shall be called inside generation callback --
 *
 *        Add a boolean array.
 *
 * @param ctx   The cb context object
 * @param name  The name of the field
 * @param array A boolean array
 * @param size  The size of the array
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_NO_MEM if some memory could not be allocated.
 */
esp_err_t json_gen_add_bool_array(const json_gen_cb_ctx_t *ctx, const char *const name,
                                  const bool *const array, size_t size);

/**
 * @brief -- Shall be called inside generation callback --
 *
 *        Add an integer value.
 *
 * @param ctx   The cb context object
 * @param name  The name of the field
 * @param value The integer value
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_NO_MEM if some memory could not be allocated.
 */
esp_err_t json_gen_add_int(const json_gen_cb_ctx_t *ctx, const char *const name, int value);

/**
 * @brief -- Shall be called inside generation callback --
 *
 *        Add an integer array.
 *
 * @param ctx   The cb context object
 * @param name  The name of the field
 * @param array An integer array
 * @param size  The size of the array
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_NO_MEM if some memory could not be allocated.
 */
esp_err_t json_gen_add_int_array(const json_gen_cb_ctx_t *ctx, const char *const name,
                                 const int *const array, size_t size);

/**
 * @brief -- Shall be called inside generation callback --
 *
 *        Add a double value.
 *
 * @param ctx   The cb context object
 * @param name  The name of the field
 * @param value The double value
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_NO_MEM if some memory could not be allocated.
 */
esp_err_t json_gen_add_double(const json_gen_cb_ctx_t *ctx, const char *const name, double value);

/**
 * @brief -- Shall be called inside generation callback --
 *
 *        Add a double array.
 *
 * @param ctx   The cb context object
 * @param name  The name of the field
 * @param array A double array
 * @param size  The size of the array
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_NO_MEM if some memory could not be allocated.
 */
esp_err_t json_gen_add_double_array(const json_gen_cb_ctx_t *ctx, const char *const name,
                                    const double *const array, size_t size);

/**
 * @brief -- Shall be called inside generation callback --
 *
 *        Add a string.
 *
 * @param ctx   The cb context object
 * @param name  The name of the field
 * @param str   The string
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_NO_MEM if some memory could not be allocated.
 */
esp_err_t json_gen_add_string(const json_gen_cb_ctx_t *ctx, const char *const name, const char *str);

/**
 * @brief -- Shall be called inside generation callback --
 *
 *        Add a string array.
 *
 * @param ctx   The cb context object
 * @param name  The name of the field
 * @param array A string array
 * @param size  The size of the array
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_NO_MEM if some memory could not be allocated.
 */
esp_err_t json_gen_add_string_array(const json_gen_cb_ctx_t *ctx, const char *const name,
                                    const char *const *array, size_t size);

/**
 * @brief -- Shall be called inside generation callback --
 *
 *        Create a generic array.
 *
 *        A new context object is provided to use for items inside the array.
 *
 * @param ctx       The cb context object
 * @param name      The name of the field
 * @param array_ctx The context to be used for items inside the array
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_NO_MEM if some memory could not be allocated.
 */
esp_err_t json_gen_add_array(const json_gen_cb_ctx_t *ctx, const char *const name,
                             json_gen_cb_ctx_t *array_ctx);

/**
 * @brief -- Shall be called inside generation callback --
 *
 *        Create a generic object.
 *
 *        A new context object is provided to use for items inside the object.
 *
 * @param ctx       The cb context object
 * @param name      The name of the field
 * @param array_ctx The context to be used for items inside the object
 *
 * @return ESP_OK if no error.
 *         ESP_ERR_NO_MEM if some memory could not be allocated.
 */
esp_err_t json_gen_add_object(const json_gen_cb_ctx_t *ctx, const char *const name,
                              json_gen_cb_ctx_t *object_ctx);
