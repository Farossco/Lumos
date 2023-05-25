/**
 * @file json_gen.c
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-25
 * 
 * @brief 
 */

#include "json_gen.h"
#include <esp_log.h>

static const char *TAG = "json";

static SLIST_HEAD( , json_gen_sub_data) sub_list;

esp_err_t json_gen_get(char **output, json_gen_type_id_t type_id, bool format)
{
	assert(output);

	json_gen_cb_ctx_t ctx;
	esp_err_t err;
	esp_err_t ret = ESP_OK;
	struct json_gen_sub_data *browse_obj;
	char error_str[100];
	cJSON *root = NULL;

	*output = NULL;

	root = cJSON_CreateObject();
	if (!root) {
		snprintf(error_str, sizeof(error_str), "Failed to create root object");
		ESP_LOGE(TAG, "Failed to create root object");
		ret = ESP_ERR_NO_MEM;
		goto end;
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

	SLIST_FOREACH(browse_obj, &sub_list, sle) {
		if (browse_obj->type_id != type_id) {
			continue;
		}

		ctx.sub_data = browse_obj;
		ctx.root     = cJSON_AddObjectToObject(root, browse_obj->name);
		if (!ctx.root) {
			snprintf(error_str, sizeof(error_str), "Failed to add item to object");
			ESP_LOGE(TAG, "Failed to add item to object");
			ret = ESP_ERR_NO_MEM;
			goto end;
		}

		err = browse_obj->cb(&ctx, browse_obj->user_data);
		if (err) {
			snprintf(error_str, sizeof(error_str), "Generation aborted by %s: %s", browse_obj->name, esp_err_to_name(err));
			ESP_LOGE(TAG, "Generation aborted by %s: %s", browse_obj->name, esp_err_to_name(err));
			ret = err;
			goto end;
		}
	}

	*output = format ? cJSON_Print(root) : cJSON_PrintUnformatted(root);
	if (!*output) {
		snprintf(error_str, sizeof(error_str), "Failed to print JSON, probably an allocation issue");
		ESP_LOGE(TAG, "Failed to print JSON, probably an allocation issue");
		ret = ESP_ERR_NO_MEM;
		goto end;
	}

end:
	cJSON_Delete(root);

	if (ret != ESP_OK) {
		assert(!*output); /* Output cannot be non-NULL at this point */
		json_gen_get_error(output, error_str, format);
	}

	return ret;
}

esp_err_t json_gen_get_error(char **output, const char *error_str, bool format)
{
	assert(output);
	assert(error_str);

	esp_err_t ret = ESP_OK;
	cJSON *root   = NULL;

	root = cJSON_CreateObject();
	if (!root) {
		ESP_LOGE(TAG, "Failed to create root object");
		ret = ESP_ERR_NO_MEM;
		goto end;
	}

	/******* Status *******/
	if (!cJSON_AddStringToObject(root, "Status", "Error")) {
		ESP_LOGE(TAG, "Failed to add item to object");
		ret = ESP_ERR_NO_MEM;
		goto end;
	}

	/******* Message *******/
	if (!cJSON_AddStringToObject(root, "Message", error_str)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		ret = ESP_ERR_NO_MEM;
		goto end;
	}

	*output = format ? cJSON_Print(root) : cJSON_PrintUnformatted(root);
	if (!*output) {
		ESP_LOGE(TAG, "Failed to print error JSON, probably an allocation issue");
		ret = ESP_ERR_NO_MEM;
		goto end;
	}

end:
	if (ret) {
		*output = NULL;
	}

	cJSON_Delete(root);

	return ret;
}

esp_err_t json_gen_sub(struct json_gen_sub_data *sub_data)
{
	assert(sub_data);
	assert(sub_data->cb);

	struct json_gen_sub_data *browse_obj;

	/* Browse the list to check for duplicate */
	SLIST_FOREACH(browse_obj, &sub_list, sle) {
		if (browse_obj == sub_data) {
			ESP_LOGE(TAG, "Subscriber %s (%p) already in the list", sub_data->name, sub_data);
			return ESP_ERR_INVALID_STATE;
		}
	}

	/* Add the subscriber at the head of the list */
	SLIST_INSERT_HEAD(&sub_list, sub_data, sle);

	ESP_LOGD(TAG, "Successfully subscribed '%s' to json type %d", sub_data->name, sub_data->type_id);

	return ESP_OK;
}

esp_err_t json_gen_add_bool(const json_gen_cb_ctx_t *ctx, const char *const name, bool value)
{
	assert(ctx);

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

esp_err_t json_gen_add_bool_array(const json_gen_cb_ctx_t *ctx, const char *const name, const bool *array, size_t size)
{
	assert(ctx);

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

esp_err_t json_gen_add_int(const json_gen_cb_ctx_t *ctx, const char *const name, int value)
{
	/* Int end up as double in cJSON anyway */
	return json_gen_add_double(ctx, name, value);
}

esp_err_t json_gen_add_int_array(const json_gen_cb_ctx_t *ctx, const char *const name,
                                 const int *const array, size_t size)
{
	assert(ctx);

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

esp_err_t json_gen_add_double(const json_gen_cb_ctx_t *ctx, const char *const name, double value)
{
	assert(ctx);

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

esp_err_t json_gen_add_double_array(const json_gen_cb_ctx_t *ctx, const char *const name,
                                    const double * const array, size_t size)
{
	assert(ctx);

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

esp_err_t json_gen_add_string(const json_gen_cb_ctx_t *ctx, const char *const name, const char *str)
{
	assert(ctx);

	cJSON *item = cJSON_CreateString(str);

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

esp_err_t json_gen_add_string_array(const json_gen_cb_ctx_t *ctx, const char *const name,
                                    const char *const *array, size_t size)
{
	assert(ctx);

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

esp_err_t json_gen_add_array(const json_gen_cb_ctx_t *ctx, const char *const name,
                             json_gen_cb_ctx_t *array_ctx)
{
	assert(ctx);
	assert(array_ctx);

	cJSON *item = cJSON_AddArrayToObject(ctx->root, name);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return ESP_ERR_NO_MEM;
	}

	array_ctx->root     = item;
	array_ctx->sub_data = ctx->sub_data;

	return ESP_OK;
}

esp_err_t json_gen_add_object(const json_gen_cb_ctx_t *ctx, const char *const name,
                              json_gen_cb_ctx_t *object_ctx)
{
	assert(ctx);
	assert(object_ctx);

	cJSON *item = cJSON_AddObjectToObject(ctx->root, name);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return ESP_ERR_NO_MEM;
	}

	object_ctx->root     = item;
	object_ctx->sub_data = ctx->sub_data;

	return ESP_OK;
}
