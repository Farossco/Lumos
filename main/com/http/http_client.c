#include "http_client.h"
#include <cJSON.h>
#include <sys/param.h>
#include <esp_http_client.h>
#include <esp_tls.h>
#include "utils.h"

static const char *TAG = "http_client";

static esp_err_t http_client_event_handler(esp_http_client_event_t *evt)
{
	static int output_len; /* Stores number of bytes read */

	switch (evt->event_id) {
	case HTTP_EVENT_ERROR:
		ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
		output_len = 0;
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

		/* TODO: Replace passing a buffer only to passing a struct containing
		 * the buffer and the size of the buffer */
		if (evt->user_data) {
			/* Copy the response into the buffer */
			int copy_len    = MIN(evt->data_len, (2048 - output_len));
			char *user_data = (char *)evt->user_data;
			if (copy_len) {
				memcpy(user_data + output_len, evt->data, copy_len);
			}

			output_len += copy_len;
		}

		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
		output_len = 0;
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
		int mbedtls_err = 0;
		esp_err_t err   = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
		if (err != 0) {
			ESP_LOGI(TAG, "Last esp error code: 0x%x",  err);
			ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
		}

		output_len = 0;
		break;

	case HTTP_EVENT_REDIRECT:
		ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
		esp_http_client_set_header(evt->client, "From",   "user@example.com");
		esp_http_client_set_header(evt->client, "Accept", "text/html");
		esp_http_client_set_redirection(evt->client);
		break;
	}
	return ESP_OK;
}

/**
 * @brief Not used anymore, keeped as a reusable template
 *
 */
void http_client_time_get(void)
{
	char local_response_buffer[200] = { 0 };
	esp_err_t err;
	cJSON *json_root, *json_status, *json_timestamp;
	esp_http_client_handle_t client;
	esp_http_client_config_t config = {
		.host          = "[TIME_HOST]",
		.path          = "/v2.1/get-time-zone",
		.query         = "format=" "[TIME_FORMAT]" "&key=" "[TIME_KEY]" "&by=" "[TIME_BY]" "&zone=" "[TIME_ZONE]" "&fields=" "[TIME_FIELDS]",
		.event_handler = http_client_event_handler,
		.user_data     = local_response_buffer, /* Pass address of local buffer to get response */
	};

	ESP_LOGI(TAG, "Connecting to server");

	client = esp_http_client_init(&config);

	err = esp_http_client_perform(client);
	if (err == ESP_OK) {
		ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
		         esp_http_client_get_status_code(client),
		         esp_http_client_get_content_length(client));
	} else {
		ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
		return;
	}
	ESP_LOGD(TAG, "HTTP response:\n========\n%s\n========", local_response_buffer);

	json_root      = cJSON_Parse(local_response_buffer);
	json_status    = cJSON_GetObjectItemCaseSensitive(json_root, "status");
	json_timestamp = cJSON_GetObjectItemCaseSensitive(json_root, "timestamp");

	if (!cJSON_IsString(json_status) && !cJSON_IsNumber(json_timestamp)) {
		ESP_LOGE(TAG, "Received invalid JSON!");
		return;
	}

	ESP_LOGD(TAG, "Status : |%s|", json_status->valuestring);

	if (strcmp(json_status->valuestring, "OK") != 0) {
		ESP_LOGE(TAG, "Wrong status, aborting");
		return;
	}

	ESP_LOGD(TAG, "Timestamp : |%lld|", (time_t)json_timestamp->valuedouble);

	esp_http_client_cleanup(client);
}
