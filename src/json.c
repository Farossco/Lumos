#include "json.h"
#include <cJSON.h>
#include <errno.h>
#include "utils.h"

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

	cJSON *root;

	root = cJSON_CreateObject();
	if (!root) {
		ESP_LOGE(TAG, "Failed to create root object");
		return -ENOSPC;
	}

	/******* Status *******/
	if (!cJSON_AddStringToObject(root, "Status", "Ok")) {
		ESP_LOGE(TAG, "Failed to add item to object");
		ret = -ENOSPC;
		goto end;
	}

	/******* Message *******/
	if (!cJSON_AddStringToObject(root, "Message", "")) {
		ESP_LOGE(TAG, "Failed to add item to object");
		ret = -ENOSPC;
		goto end;
	}

	for (struct json_sub_data *sub_data = sub_list[type]; sub_data != NULL; sub_data = sub_data->next) {
		ctx.sub_data = sub_data;
		ctx.root     = cJSON_AddObjectToObject(root, sub_data->sub_name);
		if (!ctx.root) {
			ESP_LOGE(TAG, "Failed to add item to object");
			ret = -ENOSPC;
			goto end;
		}

		err = sub_data->json_generate_cb(&ctx);
		if (err) {
			ESP_LOGE(TAG, "Generation aborted by %s: %s", sub_data->sub_name, err2str(err));
			ret = err;
			goto end;
		}
	}

	if (!cJSON_PrintPreallocated(root, buf, size, format)) {
		ESP_LOGE(TAG, "Failed to print JSON, buffer is probably too small");
		ret = -ENOMEM;
		goto end;
	}

end:
	cJSON_Delete(root);
	return ret;
}

esp_err_t json_get_error(char *buf, size_t size, const char *error_str, bool format)
{
	struct json_subscribe_obj *obj;
	json_callback_ctx_t ctx;
	esp_err_t err;
	esp_err_t ret = 0;

	cJSON *root;

	root = cJSON_CreateObject();
	if (!root) {
		ESP_LOGE(TAG, "Failed to create root object");
		return -ENOSPC;
	}

	/******* Status *******/
	if (!cJSON_AddStringToObject(root, "Status", "Error")) {
		ESP_LOGE(TAG, "Failed to add item to object");
		ret = -ENOSPC;
		goto end;
	}

	/******* Message *******/
	if (!cJSON_AddStringToObject(root, "Message", error_str)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		ret = -ENOSPC;
		goto end;
	}

	if (!cJSON_PrintPreallocated(root, buf, size, format)) {
		ESP_LOGE(TAG, "Failed to print JSON");
		ret = -ENOMEM;
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
			ESP_LOGE(TAG, "Subscriber %s (%X) already in the list", obj->sub_name, obj);
			return -EEXIST;
		}
		cur = &((*cur)->next);
	}

	*cur = obj;

	return 0;
}

esp_err_t json_subscribe_to(enum json_type type, struct json_sub_data *obj)
{
	esp_err_t err;

	if (!obj || type >= JSON_TYPE_N || !obj->json_generate_cb) {
		return -ENOENT;
	}

	err = json_sub_list_add(type, obj);
	if (err) {
		return err;
	}

	/* Object has been added at the end of the list, set it as last item */
	obj->next = NULL;

	return 0;
}

void json_dump_list(void)
{
	for (uint8_t i = 0; i < ARRAY_SIZE(sub_list); i++) {
		ESP_LOGI(TAG, "JSON list for type %d", i);

		uint8_t j = 0;

		for (struct json_sub_data *sub_data = sub_list[i]; sub_data != NULL; sub_data = sub_data->next) {
			ESP_LOGI(TAG, "  Index %d:", j);
			ESP_LOGI(TAG, "    Callback:  %X", sub_data->json_generate_cb);
			ESP_LOGI(TAG, "    Next:      %X", sub_data->next);
			ESP_LOGI(TAG, "    Sub name:  %s", sub_data->sub_name);
			j++;
		}
	}
}

esp_err_t json_gen_add_bool(json_callback_ctx_t *ctx, const char *const name, bool value)
{
	cJSON *item = cJSON_CreateBool(value);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return -ENOSPC;
	}

	if (!cJSON_AddItemToObject(ctx->root, name, item)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		return -ENOSPC;
	}

	return 0;
}

esp_err_t json_gen_add_bool_array(json_callback_ctx_t *ctx, const char *const name, const bool *array, size_t size)
{
	cJSON *item = cJSON_CreateArray();

	if (!item) {
		ESP_LOGE(TAG, "Failed to create array");
		return -ENOSPC;
	}

	for (size_t i = 0; i < size; i++) {
		cJSON *array_item = cJSON_CreateBool(array[i]);
		if (!array_item) {
			ESP_LOGE(TAG, "Failed to create object");
			return -ENOSPC;
		}

		if (!cJSON_AddItemToArray(item, array_item)) {
			ESP_LOGE(TAG, "Failed to add item to array");
			return -ENOSPC;
		}
	}

	return 0;
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
		return -ENOSPC;
	}

	if (!cJSON_AddItemToObject(ctx->root, name, item)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		return -ENOSPC;
	}

	return 0;
}

esp_err_t json_gen_add_double(json_callback_ctx_t *ctx, const char *const name, double value)
{
	cJSON *item = cJSON_CreateNumber(value);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return -ENOSPC;
	}

	if (!cJSON_AddItemToObject(ctx->root, name, item)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		return -ENOSPC;
	}

	return 0;
}

esp_err_t json_gen_add_double_array(json_callback_ctx_t *ctx, const char *const name, const double *array, size_t size)
{
	cJSON *item = cJSON_CreateDoubleArray(array, size);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return -ENOSPC;
	}

	if (!cJSON_AddItemToObject(ctx->root, name, item)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		return -ENOSPC;
	}

	return 0;
}

esp_err_t json_gen_add_string(json_callback_ctx_t *ctx, const char *const name, const char *value)
{
	cJSON *item = cJSON_CreateString(value);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return -ENOSPC;
	}

	if (!cJSON_AddItemToObject(ctx->root, name, item)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		return -ENOSPC;
	}

	return 0;
}

esp_err_t json_gen_add_string_array(json_callback_ctx_t *ctx, const char *const name, const char *const *array, size_t size)
{
	cJSON *item = cJSON_CreateStringArray(array, size);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return -ENOSPC;
	}

	if (!cJSON_AddItemToObject(ctx->root, name, item)) {
		ESP_LOGE(TAG, "Failed to add item to object");
		return -ENOSPC;
	}

	return 0;
}

esp_err_t json_gen_add_generic_array(json_callback_ctx_t *ctx, const char *const name, json_generate_cb_t array_generate_cb)
{
	json_callback_ctx_t array_ctx;
	esp_err_t err;

	cJSON *item = cJSON_AddArrayToObject(ctx->root, name);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return -ENOSPC;
	}

	array_ctx.root     = item;
	array_ctx.sub_data = ctx->sub_data;

	err = array_generate_cb(&array_ctx);
	if (err) {
		ESP_LOGE(TAG, "Array generation aborted by %s: %s", ctx->sub_data->sub_name, err2str(err));
		return err;
	}

	return 0;
}

esp_err_t json_gen_add_generic_object(json_callback_ctx_t *ctx, const char *const name, json_generate_cb_t array_generate_cb)
{
	json_callback_ctx_t object_ctx;
	esp_err_t err;

	cJSON *item = cJSON_AddObjectToObject(ctx->root, name);

	if (!item) {
		ESP_LOGE(TAG, "Failed to create object");
		return -ENOSPC;
	}

	object_ctx.root     = item;
	object_ctx.sub_data = ctx->sub_data;

	err = array_generate_cb(&object_ctx);
	if (err) {
		ESP_LOGE(TAG, "Object generation aborted by %s: %s", ctx->sub_data->sub_name, err2str(err));
		return err;
	}

	return 0;
}
