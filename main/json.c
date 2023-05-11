#include "json.h"
#include <cJSON.h>
#include "utils.h"

#include "sys/queue.h" /* TODO: Use this instead (slist) */

static const char *TAG = "json";

static struct json_sub_data *sub_list[JSON_TYPE_N];

typedef struct json_callback_ctx {
	cJSON                *root;
	struct json_sub_data *sub_data;
} json_callback_ctx_t;

esp_err_t json_get(enum json_type type, char *buf, size_t size, bool format)
{
	json_callback_ctx_t ctx;
	esp_err_t err;
	esp_err_t ret = 0;
	char error_str[100];

	cJSON *root;

	assert(buf);

	if (type >= JSON_TYPE_N) {
		snprintf(buf, size, "Json type %d is invalid (max is %d)", type, JSON_TYPE_N);
		ESP_LOGE(TAG, "Json type %d is invalid (max is %d)", type, JSON_TYPE_N);
		return ESP_ERR_INVALID_ARG;
	}

	root = cJSON_CreateObject();
	if (!root) {
		snprintf(buf, size, "Failed to create root object");
		ESP_LOGE(TAG, "Failed to create root object");
		return ESP_ERR_NO_MEM;
	}

	/******* Status *******/
	if (!cJSON_AddStringToObject(root, "Status", "OK")) {
		snprintf(error_str, sizeof(error_str), "Failed to add item to object");
		ESP_LOGE(TAG, "Failed to add item to object");
		ret = ESP_ERR_NO_MEM;
		goto end;
	}

	/******* Message *******/
	if (!cJSON_AddStringToObject(root, "Message", "")) {
		snprintf(error_str, sizeof(error_str), "Failed to add item to object");
		ESP_LOGE(TAG, "Failed to add item to object");
		ret = ESP_ERR_NO_MEM;
		goto end;
	}

	for (struct json_sub_data *sub_data = sub_list[type]; sub_data != NULL; sub_data = sub_data->next) {
		ctx.sub_data = sub_data;
		ctx.root     = cJSON_AddObjectToObject(root, sub_data->sub_name);
		if (!ctx.root) {
			snprintf(error_str, sizeof(error_str), "Failed to add item to object");
			ESP_LOGE(TAG, "Failed to add item to object");
			ret = ESP_ERR_NO_MEM;
			goto end;
		}

		err = sub_data->json_generate_cb(&ctx);
		if (err) {
			snprintf(error_str, sizeof(error_str), "Generation aborted by %s: %s", sub_data->sub_name, esp_err_to_name(err));
			ESP_LOGE(TAG, "Generation aborted by %s: %s", sub_data->sub_name, esp_err_to_name(err));
			ret = err;
			goto end;
		}
	}

	if (!cJSON_PrintPreallocated(root, buf, size, format)) {
		snprintf(buf, size, "Failed to print JSON, buffer is probably too small");
		ESP_LOGE(TAG, "Failed to print JSON, buffer is probably too small");
		cJSON_Delete(root);
		return ESP_ERR_NO_MEM;
	}

end:
	cJSON_Delete(root);

	if (ret != ESP_OK) {
		err = json_get_error(buf, size, error_str, format);
		if (err) {
			return err;
		}
	}

	return ret;
}

esp_err_t json_get_error(char *buf, size_t size, const char *error_str, bool format)
{
	esp_err_t ret = ESP_OK;

	cJSON *root;

	root = cJSON_CreateObject();
	if (!root) {
		snprintf(buf, size, "Failed to create root object");
		ESP_LOGE(TAG, "Failed to create root object");
		return ESP_ERR_NO_MEM;
	}

	/******* Status *******/
	if (!cJSON_AddStringToObject(root, "Status", "Error")) {
		snprintf(buf, size, "Failed to add item to object");
		ESP_LOGE(TAG, "Failed to add item to object");
		ret = ESP_ERR_NO_MEM;
		goto end;
	}

	/******* Message *******/
	if (!cJSON_AddStringToObject(root, "Message", error_str)) {
		snprintf(buf, size, "Failed to add item to object");
		ESP_LOGE(TAG, "Failed to add item to object");
		ret = ESP_ERR_NO_MEM;
		goto end;
	}

	if (!cJSON_PrintPreallocated(root, buf, size, format)) {
		snprintf(buf, size, "Failed to print JSON, buffer is probably too small");
		ESP_LOGE(TAG, "Failed to print JSON, buffer is probably too small");
		ret = ESP_ERR_NO_MEM;
		goto end;
	}

end:
	cJSON_Delete(root);
	return ret;
}

static esp_err_t json_sub_list_add(enum json_type type, struct json_sub_data *obj)
{
	struct json_sub_data **cur = &sub_list[type];

	while (*cur != NULL) {
		if (*cur == obj) {
			ESP_LOGE(TAG, "Subscriber %s (%p) already in the list", obj->sub_name, obj);
			return ESP_ERR_INVALID_STATE;
		}
		cur = &((*cur)->next);
	}

	*cur = obj;

	return ESP_OK;
}

esp_err_t json_subscribe_to(enum json_type type, struct json_sub_data *obj)
{
	esp_err_t err;

	if (!obj || type >= JSON_TYPE_N || !obj->json_generate_cb) {
		return ESP_ERR_INVALID_ARG;
	}

	err = json_sub_list_add(type, obj);
	if (err) {
		return err;
	}

	/* Object has been added at the end of the list, set it as last item */
	obj->next = NULL;

	return ESP_OK;
}

esp_err_t json_gen_add_bool(json_callback_ctx_t *ctx, const char *const name, bool value)
{
	cJSON *item = cJSON_CreateBool(value);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return ESP_ERR_NO_MEM;
	}

	if (!cJSON_AddItemToObject(ctx->root, name, item)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		return ESP_ERR_NO_MEM;
	}

	return ESP_OK;
}

esp_err_t json_gen_add_bool_array(json_callback_ctx_t *ctx, const char *const name, const bool *array, size_t size)
{
	cJSON *item = cJSON_CreateArray();

	if (!item) {
		ESP_LOGE(TAG, "Failed to create array");
		return ESP_ERR_NO_MEM;
	}

	for (size_t i = 0; i < size; i++) {
		cJSON *array_item = cJSON_CreateBool(array[i]);
		if (!array_item) {
			ESP_LOGE(TAG, "Failed to create object");
			return ESP_ERR_NO_MEM;
		}

		if (!cJSON_AddItemToArray(item, array_item)) {
			ESP_LOGE(TAG, "Failed to add item to array");
			return ESP_ERR_NO_MEM;
		}
	}

	return ESP_OK;
}

esp_err_t json_gen_add_int(json_callback_ctx_t *ctx, const char *const name, int value)
{
	/* Int end up as double in cJSON anyway */
	return json_gen_add_double(ctx, name, value);
}

esp_err_t json_gen_add_int_array(json_callback_ctx_t *ctx, const char *const name, const int *array, size_t size)
{
	cJSON *item = cJSON_CreateIntArray(array, size);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return ESP_ERR_NO_MEM;
	}

	if (!cJSON_AddItemToObject(ctx->root, name, item)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		return ESP_ERR_NO_MEM;
	}

	return ESP_OK;
}

esp_err_t json_gen_add_double(json_callback_ctx_t *ctx, const char *const name, double value)
{
	cJSON *item = cJSON_CreateNumber(value);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return ESP_ERR_NO_MEM;
	}

	if (!cJSON_AddItemToObject(ctx->root, name, item)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		return ESP_ERR_NO_MEM;
	}

	return ESP_OK;
}

esp_err_t json_gen_add_double_array(json_callback_ctx_t *ctx, const char *const name, const double *array, size_t size)
{
	cJSON *item = cJSON_CreateDoubleArray(array, size);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return ESP_ERR_NO_MEM;
	}

	if (!cJSON_AddItemToObject(ctx->root, name, item)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		return ESP_ERR_NO_MEM;
	}

	return ESP_OK;
}

esp_err_t json_gen_add_string(json_callback_ctx_t *ctx, const char *const name, const char *value)
{
	cJSON *item = cJSON_CreateString(value);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return ESP_ERR_NO_MEM;
	}

	if (!cJSON_AddItemToObject(ctx->root, name, item)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		return ESP_ERR_NO_MEM;
	}

	return ESP_OK;
}

esp_err_t json_gen_add_string_array(json_callback_ctx_t *ctx, const char *const name, const char *const *array, size_t size)
{
	cJSON *item = cJSON_CreateStringArray(array, size);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return ESP_ERR_NO_MEM;
	}

	if (!cJSON_AddItemToObject(ctx->root, name, item)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		return ESP_ERR_NO_MEM;
	}

	return ESP_OK;
}

esp_err_t json_gen_add_generic_array(json_callback_ctx_t *ctx, const char *const name, json_generate_cb_t array_generate_cb)
{
	json_callback_ctx_t array_ctx;
	esp_err_t err;

	cJSON *item = cJSON_AddArrayToObject(ctx->root, name);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return ESP_ERR_NO_MEM;
	}

	array_ctx.root     = item;
	array_ctx.sub_data = ctx->sub_data;

	err = array_generate_cb(&array_ctx);
	if (err) {
		ESP_LOGE(TAG, "Array generation aborted by %s: %s", ctx->sub_data->sub_name, esp_err_to_name(err));
		return err;
	}

	return ESP_OK;
}

esp_err_t json_gen_add_generic_object(json_callback_ctx_t *ctx, const char *const name, json_generate_cb_t array_generate_cb)
{
	json_callback_ctx_t object_ctx;
	esp_err_t err;

	cJSON *item = cJSON_AddObjectToObject(ctx->root, name);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return ESP_ERR_NO_MEM;
	}

	object_ctx.root     = item;
	object_ctx.sub_data = ctx->sub_data;

	err = array_generate_cb(&object_ctx);
	if (err) {
		ESP_LOGE(TAG, "Object generation aborted by %s: %s", ctx->sub_data->sub_name, esp_err_to_name(err));
		return err;
	}

	return ESP_OK;
}
