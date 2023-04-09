#include "wifi_com.h"

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <sys/param.h>
#include "esp_tls.h"
#include "Request.h"
#include "Json.h"
#include "esp_http_server.h"
#include "esp_http_client.h"
#include "esp_event.h"
#include "temp_log_util.h"
#include "nvs_flash.h"

#define TIME_HOST   "api.timezonedb.com" /* HTPP time host */
#define TIME_FORMAT "json"               /* Format for receive the time */
#define TIME_KEY    ESP_TIME_API_KEY     /* TimeZoneDB API key */
#define TIME_BY     "zone"               /* localization method */
#define TIME_FIELDS "timestamp"          /* Needed fields in the answer */

static const struct wifi_com_conn_callbacks *conn_callbacks;
static httpd_handle_t handle;
static const httpd_config_t config = HTTPD_DEFAULT_CONFIG();

static const char *TAG = "wifi_com";

static bool load_from_spiffs(httpd_req_t *rqst, String path)
{
	const char *data_type = "text/plain";
	bool file_found       = false;

	if (path.endsWith("/")) {
		path += "index.html";
	}

/*
 *  if (rqst->hasArg("download")) {
 *      data_type = "application/octet-stream";
 *  }
 */

	if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
	else if (path.endsWith(".html")) data_type = "text/html";
	else if (path.endsWith(".htm")) data_type = "text/html";
	else if (path.endsWith(".css")) data_type = "text/css";
	else if (path.endsWith(".js")) data_type = "application/javascript";
	else if (path.endsWith(".png")) data_type = "image/png";
	else if (path.endsWith(".gif")) data_type = "image/gif";
	else if (path.endsWith(".jpg")) data_type = "image/jpeg";
	else if (path.endsWith(".ico")) data_type = "image/x-icon";
	else if (path.endsWith(".xml")) data_type = "text/xml";
	else if (path.endsWith(".pdf")) data_type = "application/pdf";
	else if (path.endsWith(".zip")) data_type = "application/zip";


	if (SPIFFS.exists(path)) {
		File file   = SPIFFS.open(path);
		String data = file.readString();

		httpd_resp_set_type(rqst, data_type);
		httpd_resp_send(rqst, data.c_str(), data.length());
		file_found = true;
	} else {
		ESP_LOGE("File %s doesn't exist\n", path.c_str());
	}

	return file_found;
}

static void request_display(httpd_req_t *rqst)
{
	ESP_LOGD(TAG, "Received request\n");

	ESP_LOGD(TAG, "Request: |%s|\n", rqst->uri);

	/*
	 * for (int i = 0; i < rqst->args(); i++) {
	 * 	trace << " |" << rqst->argName(i) << "=" << rqst->arg(i) << "|";
	 * }
	 */
}

static esp_err_t handle_command(httpd_req_t *rqst)
{
	char query[500];
	size_t query_length;
	String message;
	char type[10]  = { 0 };
	char comp[10]  = { 0 };
	char value[10] = { 0 };

	request_display(rqst);

	httpd_req_get_url_query_str(rqst, query, sizeof(query));
	httpd_query_key_value(query, "type",  type,  sizeof(type));
	httpd_query_key_value(query, "comp",  comp,  sizeof(comp));
	httpd_query_key_value(query, "value", value, sizeof(value));

	Request request(type, comp, value);

	request.process();

	if (request.getError()) {
		message = json.getErrorPretty(request.getError());
	} else {
		if (request.getType() != RequestType::requestData) {
			String data = request.getType().toString(true) +
			              request.getComplementString() +
			              request.getInformationString() + 'z';
		}

		message = json.getDataPretty();
	}

	httpd_resp_set_type(rqst, "application/json");
	httpd_resp_send(rqst, message.c_str(), message.length());

	/* rqst->send(200, "application/json", message); */

	return ESP_OK;
}

static esp_err_t handle_get_res(httpd_req_t *rqst)
{
	request_display(rqst);

	String message = json.getResourcesPretty();

	httpd_resp_set_type(rqst, "application/json");
	httpd_resp_send(rqst, message.c_str(), message.length());

	return ESP_OK;
}

static esp_err_t handle_web_requests(httpd_req_t *rqst, httpd_err_code_t error)
{
	char query[500];
	size_t query_length;

	request_display(rqst);

	if (load_from_spiffs(rqst, rqst->uri)) {
		return ESP_OK;
	}

	String message = "File Not Detected\n\n";

	query_length = httpd_req_get_url_query_len(rqst);
	httpd_req_get_url_query_str(rqst, query, sizeof(query));

	message += String("URL: ") + rqst->uri + "\n";
	message += String("Method: ") + ((rqst->method == HTTP_GET) ? "GET" : "POST") + "\n";
	message += String("Query length: ") + query_length + "\n";
	message += String("Query: ") + query + "\n";

	httpd_resp_set_status(rqst, HTTPD_404);
	httpd_resp_set_type(rqst, "text/plain");
	httpd_resp_send(rqst, message.c_str(), message.length());

	return ESP_OK;
}

/* URI handler structure */
httpd_uri_t command_uri {
	.uri      = "/command",
	.method   = HTTP_GET,
	.handler  = handle_command,
	.user_ctx = NULL
};

/* URI handler structure */
httpd_uri_t get_res_uri {
	.uri      = "/getRes",
	.method   = HTTP_GET,
	.handler  = handle_get_res,
	.user_ctx = NULL
};

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
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

		/* Should be added once upgrading to ESP-IDF v5
		 * case HTTP_EVENT_REDIRECT:
		 * 	ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
		 * 	esp_http_client_set_header(evt->client, "From",   "user@example.com");
		 * 	esp_http_client_set_header(evt->client, "Accept", "text/html");
		 * 	esp_http_client_set_redirection(evt->client);
		 * 	break;
		 */
	}
	return ESP_OK;
}

static void wifi_com_time_get(void)
{
	const size_t capacity = JSON_OBJECT_SIZE(3) + /* root (status, message, timestamp) */
	                        50;                   /* String duplications + security margin */

	char local_response_buffer[200] = { 0 };

	/**
	 * NOTE: All the configuration parameters for http_client must be spefied either in URL or as host and path parameters.
	 * If host and path parameters are not set, query parameter will be ignored. In such cases,
	 * query parameter should be specified in URL.
	 *
	 * If URL as well as host and path parameters are specified, values of host and path will be considered.
	 */
	esp_http_client_config_t config = {
		.host          = TIME_HOST,
		.path          = "/v2.1/get-time-zone",
		.query         = "format=" TIME_FORMAT "&key=" TIME_KEY "&by=" TIME_BY "&zone=" TIME_ZONE "&fields=" TIME_FIELDS,
		.event_handler = _http_event_handler,
		.user_data     = local_response_buffer, /* Pass address of local buffer to get response */
	};

	ESP_LOGI(TAG, "Connecting to server");

	esp_http_client_handle_t client = esp_http_client_init(&config);

	/* GET */
	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK) {
		ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
		         esp_http_client_get_status_code(client),
		         esp_http_client_get_content_length(client));
	} else {
		ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
		return;
	}
	ESP_LOGD(TAG, "local_response_buffer:\n========\n%s\n========", local_response_buffer);

	DynamicJsonDocument doc(capacity);

	/* Deserialize the JSON document */
	deserializeJson(doc, local_response_buffer);

	String status    = ((const char *)doc["status"]);
	time_t timestamp = doc["timestamp"];

	ESP_LOGD(TAG, "Status : |%s|", status);

	if (status.indexOf("OK") != 0) {
		ESP_LOGE(TAG, "Wrong status, leaving. (%s)", status);
		return;
	}

	ESP_LOGD(TAG, "Timestamp : |%d|", timestamp);

	setTime(timestamp);

	ESP_LOGD(TAG, "Time set!");

	esp_http_client_cleanup(client);
}

void wifi_com_init(void)
{
	int err;
	uint32_t timer;

	SPIFFS.begin();

	err = httpd_start(&handle, &config);
	if (err == ESP_OK) {
		ESP_LOGI(TAG, "Server started!\n");
		httpd_register_uri_handler(handle, &command_uri);
		httpd_register_uri_handler(handle, &get_res_uri);
		httpd_register_err_handler(handle, HTTPD_404_NOT_FOUND, handle_web_requests);
		ESP_LOGD(TAG, "Handler registered!\n");
	} else {
		ESP_LOGE(TAG, "Failed to start server: %d\n", err);
	}

	/*
	 * server.on("/", HTTP_ANY, handle_root);
	 * server.on("/command", HTTP_ANY, handle_command);
	 * server.on("/getRes",  HTTP_ANY, handle_get_res);
	 * server.onNotFound(handle_web_requests);
	 * server.begin();
	 */

	/*
	 * trace << "Server started" << endl;
	 * trace << "Local IP: " << WiFi.localIP().toString() << endl;
	 */

	wifi_com_time_get();

	if (conn_callbacks && conn_callbacks->on_connected) {
		conn_callbacks->on_connected();
	} else {
		ESP_LOGW(TAG, "on_connect callback not set\n");
	}
}

void wifi_com_register_conn_callbacks(const struct wifi_com_conn_callbacks *callbacks)
{
	conn_callbacks = callbacks;
}
