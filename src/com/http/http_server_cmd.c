#include "http_server_cmd.h"
#include <string.h>
#include "utils.h"

static const char *TAG = "http_server_cmd";

static const struct http_server_cmd_config *http_server_cmd_config;

static esp_err_t call_callbacks(httpd_req_t *rqst, char *err_str, size_t size)
{
	char query[500];
	char type[10]      = { 0 };
	char comp[10]      = { 0 };
	char value[10]     = { 0 };
	uint32_t comp_int  = 0;
	uint32_t value_int = 0;
	char *str_end;
	esp_err_t err;
	bool handle_found = false;

	err = httpd_req_get_url_query_str(rqst, query, sizeof(query));
	if (err == ESP_ERR_NOT_FOUND) {
		query[0] = '\0';
	} else if (err) {
		ESP_LOGE(TAG, "Failed to get URL query: %s", err2str(err));
		snprintf(err_str, size, "Failed to get URL query: %s", err2str(err));
		return err;
	}

	ESP_LOGD(TAG, "Received query: %s", query);

	err = httpd_query_key_value(query, "type", type, sizeof(type));
	if (err == ESP_ERR_NOT_FOUND) {
		ESP_LOGE(TAG, "\"type\" key value not provided");
		snprintf(err_str, size, "type key value not provided");
		return err;
	} else if (err) {
		ESP_LOGE(TAG, "Failed to get \"type\" key value: %s", err2str(err));
		snprintf(err_str, size, "Failed to get type key value: %s", err2str(err));
		return err;
	}

	err = httpd_query_key_value(query, "comp", comp, sizeof(comp));
	if (err == ESP_ERR_NOT_FOUND) {
		comp_int = 0;
	} else if (err) {
		ESP_LOGE(TAG, "Failed to get \"comp\" key value: %s", err2str(err));
		snprintf(err_str, size, "Failed to get comp key value: %s", err2str(err));
		return err;
	} else {
		comp_int = strtol(comp, &str_end, 10);

		if (str_end == comp) {
			ESP_LOGD(TAG, "%s is not a correct number", comp);
			snprintf(err_str, size, "%s is not a correct number", comp);
			return ESP_ERR_INVALID_ARG;
		}
	}

	err = httpd_query_key_value(query, "value", value, sizeof(value));
	if (err == ESP_ERR_NOT_FOUND) {
		value_int = 0;
	} else if (err) {
		ESP_LOGE(TAG, "Failed to get \"value\" key value: %s", err2str(err));
		snprintf(err_str, size, "Failed to get value key value: %s", err2str(err));
		return err;
	} else {
		value_int = strtol(value, &str_end, 10);

		if (str_end == value) {
			ESP_LOGD(TAG, "%s is not a correct number", value);
			snprintf(err_str, size, "%s is not a correct number", value);
			return ESP_ERR_INVALID_ARG;
		}
	}

	for (uint8_t i = 0; i < http_server_cmd_config->cmd_handles_size; i++) {
		const http_server_cmd_handle_t *handle = &http_server_cmd_config->cmd_handles[i];

		if (strstr(type, handle->cmd_type)) {
			if (handle->on_cmd_cb) {
				err = handle->on_cmd_cb(type, comp_int, value_int);
				if (err) {
					ESP_LOGE(TAG, "Error raised by CB: %s", err2str(err));
					snprintf(err_str, size, "An error happened while handling the request: %s", err2str(err));
					return err;
				}
			}
			handle_found = true;
		}
	}

	if (!handle_found) {
		ESP_LOGI(TAG, "No handler found for \"%s\"", type);
		snprintf(err_str, size, "Incorrect type: %s", type);
		return -ESP_ERR_NOT_FOUND;
	}

	snprintf(err_str, size, "All good");

	return 0;
}

static esp_err_t http_server_cmd_handler(httpd_req_t *rqst)
{
	char err_str[200];
	size_t query_length;
	esp_err_t err;

	err = call_callbacks(rqst, err_str, sizeof(err_str));

	if (http_server_cmd_config->on_end_cb) {
		err = http_server_cmd_config->on_end_cb(rqst, err, err_str);
		{
			if (err) {
				ESP_LOGE(TAG, "An error happened during on_end_cb: %s", err2str(err));
				return err;
			}
		}
	}

	return ESP_OK;
}

/* URI handler structure */
static const httpd_uri_t http_server_cmd_uri = {
	.uri      = "/command",
	.method   = HTTP_GET,
	.handler  = http_server_cmd_handler,
	.user_ctx = NULL
};

esp_err_t http_server_cmd_register(httpd_handle_t handle, const struct http_server_cmd_config *config)
{
	httpd_register_uri_handler(handle, &http_server_cmd_uri);

	http_server_cmd_config = config;
}
