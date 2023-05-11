#ifndef LUMOS_JSON_H
#define LUMOS_JSON_H

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

#include <esp_err.h>
#include <stdint.h>
#include <stdbool.h>

#define JSON_GEN_CALL_AND_TEST(func_call) { \
		esp_err_t err = func_call;               \
		if (err) {                               \
			return err;                          \
		}                                        \
}

/* JSON creation helpers */
#define JSON_GEN_ADD_BOOL(name, value)               JSON_GEN_CALL_AND_TEST(json_gen_add_bool(ctx, name, value))
#define JSON_GEN_ADD_BOOL_ARRAY(name, array, size)   JSON_GEN_CALL_AND_TEST(json_gen_add_bool_array(ctx, name, array, size))
#define JSON_GEN_ADD_INT(name, value)                JSON_GEN_CALL_AND_TEST(json_gen_add_int(ctx, name, value))
#define JSON_GEN_ADD_INT_ARRAY(name, array, size)    JSON_GEN_CALL_AND_TEST(json_gen_add_int_array(ctx, name, array, size))
#define JSON_GEN_ADD_DOUBLE(name, value)             JSON_GEN_CALL_AND_TEST(json_gen_add_double(ctx, name, value))
#define JSON_GEN_ADD_DOUBLE_ARRAY(name, array, size) JSON_GEN_CALL_AND_TEST(json_gen_add_double_array(ctx, name, array, size))
#define JSON_GEN_ADD_STRING(name, value)             JSON_GEN_CALL_AND_TEST(json_gen_add_string(ctx, name, value))
#define JSON_GEN_ADD_STRING_ARRAY(name, array, size) JSON_GEN_CALL_AND_TEST(json_gen_add_string_array(ctx, name, array, size))
#define JSON_GEN_ADD_GENERIC_ARRAY(name, cb)         JSON_GEN_CALL_AND_TEST(json_gen_add_generic_array(ctx, name, cb))
#define JSON_GEN_ADD_GENERIC_OBJECT(name, cb)        JSON_GEN_CALL_AND_TEST(json_gen_add_generic_object(ctx, name, cb))

/* Array creation helpers */
#define JSON_GEN_ARRAY_ADD_BOOL(value)               JSON_GEN_ADD_BOOL("", value)
#define JSON_GEN_ARRAY_ADD_BOOL_ARRAY(array, size)   JSON_GEN_ADD_BOOL_ARRAY("", array, size)
#define JSON_GEN_ARRAY_ADD_INT(value)                JSON_GEN_ADD_INT("", value)
#define JSON_GEN_ARRAY_ADD_INT_ARRAY(array, size)    JSON_GEN_ADD_INT_ARRAY("", array, size)
#define JSON_GEN_ARRAY_ADD_DOUBLE(value)             JSON_GEN_ADD_DOUBLE("", value)
#define JSON_GEN_ARRAY_ADD_DOUBLE_ARRAY(array, size) JSON_GEN_ADD_DOUBLE_ARRAY("", array, size)
#define JSON_GEN_ARRAY_ADD_STRING(value)             JSON_GEN_ADD_STRING("", value)
#define JSON_GEN_ARRAY_ADD_STRING_ARRAY(array, size) JSON_GEN_ADD_STRING_ARRAY("", array, size)
#define JSON_GEN_ARRAY_ADD_GENERIC_ARRAY(cb)         JSON_GEN_ADD_GENERIC_ARRAY("", cb)
#define JSON_GEN_ARRAY_ADD_GENERIC_OBJECT(cb)        JSON_GEN_ADD_GENERIC_OBJECT("", cb)

enum json_type {
	JSON_TYPE_DATA = 0,
	JSON_TYPE_RES  = 1,

	JSON_TYPE_N
};

typedef struct json_callback_ctx json_callback_ctx_t;
typedef esp_err_t (*json_generate_cb_t)(json_callback_ctx_t *ctx);

struct json_sub_data {
	const char           *sub_name;
	json_generate_cb_t   json_generate_cb;
	struct json_sub_data *next;
};

esp_err_t json_get(enum json_type type, char *buf, size_t size, bool format);
esp_err_t json_get_error(char *buf, size_t size, const char *error_str, bool format);

esp_err_t json_subscribe_to(enum json_type type, struct json_sub_data *obj);

esp_err_t json_gen_add_bool(json_callback_ctx_t *ctx, const char *const name, bool value);
esp_err_t json_gen_add_bool_array(json_callback_ctx_t *ctx, const char *const name, const bool *const array, size_t size);
esp_err_t json_gen_add_int(json_callback_ctx_t *ctx, const char *const name, int value);
esp_err_t json_gen_add_int_array(json_callback_ctx_t *ctx, const char *const name, const int *const array, size_t size);
esp_err_t json_gen_add_double(json_callback_ctx_t *ctx, const char *const name, double value);
esp_err_t json_gen_add_double_array(json_callback_ctx_t *ctx, const char *const name, const double *const array, size_t size);
esp_err_t json_gen_add_string(json_callback_ctx_t *ctx, const char *const name, const char *value);
esp_err_t json_gen_add_string_array(json_callback_ctx_t *ctx, const char *const name, const char *const *strings, size_t size);
esp_err_t json_gen_add_generic_array(json_callback_ctx_t *ctx, const char *const name, json_generate_cb_t array_generate_cb);
esp_err_t json_gen_add_generic_object(json_callback_ctx_t *ctx, const char *const name, json_generate_cb_t object_generate_cb);

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* ifndef LUMOS_JSON_H */
