#include "http_server.h"
#include <esp_http_server.h>
#include <errno.h>
#include "http_server_cmd.h"
#include "json.h"
#include "spiffs.h"
#include "utils.h"
#include "light.h"

static httpd_handle_t httpd_handle;

static const char *TAG = "http_server";

static bool on_chunk_read(char *chunk_data, size_t chunk_size, void *arg)
{
	httpd_req_t *rqst = (httpd_req_t *)arg;
	esp_err_t err;

	err = httpd_resp_send_chunk(rqst, chunk_data, chunk_size);
	if (err) {
		ESP_LOGE(TAG, "Failed to send chunk: %s", err2str(err));
		return SPIFFS_READ_CB_STOP;
	}

	return SPIFFS_READ_CB_CONTINUE;
}

static bool load_from_spiffs(char *query, httpd_req_t *rqst, const char *path)
{
	const char *data_type = "text/plain";
	char chunk_buf[1000];
	esp_err_t err;

	if (str_endswith(path, ".html")) data_type = "text/html";
	else if (str_endswith(path, ".css")) data_type = "text/css";
	else if (str_endswith(path, ".js")) data_type = "application/javascript";
	else if (str_endswith(path, ".png")) data_type = "image/png";
	else if (str_endswith(path, ".gif")) data_type = "image/gif";
	else if (str_endswith(path, ".jpg")) data_type = "image/jpeg";
	else if (str_endswith(path, ".ico")) data_type = "image/x-icon";
	else if (str_endswith(path, ".xml")) data_type = "text/xml";
	else if (str_endswith(path, ".pdf")) data_type = "application/pdf";
	else if (str_endswith(path, ".zip")) data_type = "application/zip";

	err = spiffs_read_file(NULL, 0, path);
	if (err == ESP_ERR_NOT_FOUND && strchr(path, '/') == strrchr(path, '/')) {
		ESP_LOGD(TAG, "Redirecting %s to %s", path, "/index.html");
		path      = "/index.html";
		data_type = "text/html";
	} else if (err) {
		ESP_LOGE(TAG, "Failed to read file %s: %s", path, err2str(err));
		return false;
	}

	httpd_resp_set_type(rqst, data_type);

	err = spiffs_read_file_chunks(chunk_buf, sizeof(chunk_buf), path, on_chunk_read, rqst);
	if (err) {
		ESP_LOGE(TAG, "Failed to read file %s: %s", path, err2str(err));
		return false;
	}

	ESP_LOGV(TAG, "Response sent");

	return true;
}

static esp_err_t handle_get_res(httpd_req_t *rqst)
{
	char resp_buf[1000];
	const char *resp_type = "application/json";
	esp_err_t err;

	ESP_LOGV(TAG, "Received request");
	ESP_LOGD(TAG, "Request: |%s|", rqst->uri);

	err = json_get(JSON_TYPE_RES, resp_buf, sizeof(resp_buf), true);
	if (err) {
		ESP_LOGE(TAG, "Failed to generate JSON: %s", err2str(err));
		snprintf(resp_buf, sizeof(resp_buf), "Failed to generate JSON: %s", err2str(err));
		resp_type = "text/plain";
	}

	err = httpd_resp_set_type(rqst, resp_type);
	if (err) {
		ESP_LOGE(TAG, "Failed to set response type: %s", err2str(err));
		return err;
	}

	err = httpd_resp_sendstr(rqst, resp_buf);
	if (err) {
		ESP_LOGE(TAG, "Failed to send response: %s", err2str(err));
		return err;
	}

	return ESP_OK;
}

static esp_err_t handle_web_requests(httpd_req_t *rqst, httpd_err_code_t error)
{
	char query[500];
	char response[1000];
	size_t query_length;
	esp_err_t err;

	ESP_LOGD(TAG, "Received request");
	ESP_LOGD(TAG, "Request: |%s|", rqst->uri);

	query_length = httpd_req_get_url_query_len(rqst);

	err = httpd_req_get_url_query_str(rqst, query, sizeof(query));
	if (err == ESP_ERR_NOT_FOUND) {
		query[0] = '\0';
	} else if (err) {
		ESP_LOGE(TAG, "Failed to get url query str: %s", err2str(err));
		return err;
	}

	if (!load_from_spiffs(query, rqst, rqst->uri)) {
		ssize_t length = snprintf(response, sizeof(response),
		                          "File Not Detected\n\nURL: %s\nMethod: %s\nQuery length: %d\nQuery: %s\n",
		                          rqst->uri, (rqst->method == HTTP_GET) ? "GET" : "POST", query_length, query);

		if (length < 0) {
			snprintf(response, sizeof(response), "Encoding error while generating 404 response");
		}

		err = httpd_resp_set_status(rqst, HTTPD_404);
		if (err) {
			ESP_LOGE(TAG, "Failed to set status: %s", err2str(err));
			return err;
		}

		err = httpd_resp_set_type(rqst, "text/plain");
		if (err) {
			ESP_LOGE(TAG, "Failed to set status: %s", err2str(err));
			return err;
		}

		err = httpd_resp_sendstr(rqst, response);
		if (err) {
			ESP_LOGE(TAG, "Failed to send page: %s", err2str(err));
			return err;
		}

		ESP_LOGV(TAG, "404 page sent");
	}

	return ESP_OK;
}

static esp_err_t on_cmd_lon(const char *const cmd_type, uint32_t complement, uint32_t value)
{
	return light_state_set(value != 0);
}

static esp_err_t on_cmd_lmo(const char *const cmd_type, uint32_t complement, uint32_t value)
{
	return light_mode_set(value);
}

static esp_err_t on_cmd_rgb(const char *const cmd_type, uint32_t complement, uint32_t value)
{
	return light_color_set(rgb_from_code(value), complement);
}

static esp_err_t on_cmd_spe(const char *const cmd_type, uint32_t complement, uint32_t value)
{
	return light_speed_set(value, complement);
}

static esp_err_t on_cmd_pow(const char *const cmd_type, uint32_t complement, uint32_t value)
{
	return light_power_set(value, complement);
}

static const http_server_cmd_handle_t cmd_handles[] = {
	HTTP_SERVER_CMD_HANDLE("RQD", NULL),
	HTTP_SERVER_CMD_HANDLE("LON", on_cmd_lon),
	HTTP_SERVER_CMD_HANDLE("LMO", on_cmd_lmo),
	HTTP_SERVER_CMD_HANDLE("RGB", on_cmd_rgb),
	HTTP_SERVER_CMD_HANDLE("SPE", on_cmd_spe),
	HTTP_SERVER_CMD_HANDLE("POW", on_cmd_pow)
};

static esp_err_t http_server_cmd_send_response(httpd_req_t *rqst, const uint8_t error, const char *error_str)
{
	esp_err_t err;
	char resp_buf[1000];
	const char *resp_type = "application/json";

	if (error) {
		err = json_get_error(resp_buf, sizeof(resp_buf), error_str, true);
	} else {
		err = json_get(JSON_TYPE_DATA, resp_buf, sizeof(resp_buf), true);
	}

	if (err) {
		ESP_LOGE(TAG, "Failed to generate JSON: %s", err2str(err));
		snprintf(resp_buf, sizeof(resp_buf), "Failed to generate JSON: %s", err2str(err));
		resp_type = "text/plain";
	}

	err = httpd_resp_set_type(rqst, resp_type);
	if (err) {
		ESP_LOGE(TAG, "Failed to set response type: %s", err2str(err));
	}

	/* Necessary for the web app to be hosted on a separate server */
	err = httpd_resp_set_hdr(rqst, "Access-Control-Allow-Origin", "*");
	if (err) {
		ESP_LOGE(TAG, "Failed to set header: %s", err2str(err));
	}

	err = httpd_resp_sendstr(rqst, resp_buf);
	if (err) {
		ESP_LOGE(TAG, "Failed to send response: %s", err2str(err));
		return err;
	}

	return ESP_OK;
}

static const struct http_server_cmd_config cmd_config = {
	.cmd_handles      = cmd_handles,
	.cmd_handles_size = ARRAY_SIZE(cmd_handles),
	.on_end_cb        = http_server_cmd_send_response,
	.on_not_found_cb  = NULL
};

/* URI handler structure */
static const httpd_uri_t get_res_uri = {
	.uri      = "/getRes",
	.method   = HTTP_GET,
	.handler  = handle_get_res,
	.user_ctx = NULL
};

void http_server_start(void)
{
	esp_err_t err;

	const httpd_config_t config = {
		.task_priority                = tskIDLE_PRIORITY + 5,
		.stack_size                   = 16384,
		.core_id                      = CONFIG_LUMOS_COM_CORE_ID,
		.server_port                  = 80,
		.ctrl_port                    = 32768,
		.max_open_sockets             = 7,
		.max_uri_handlers             = 8,
		.max_resp_headers             = 8,
		.backlog_conn                 = 5,
		.lru_purge_enable             = false,
		.recv_wait_timeout            = 5,
		.send_wait_timeout            = 5,
		.global_user_ctx              = NULL,
		.global_user_ctx_free_fn      = NULL,
		.global_transport_ctx         = NULL,
		.global_transport_ctx_free_fn = NULL,
		.enable_so_linger             = false,
		.linger_timeout               = 0,
		.open_fn                      = NULL,
		.close_fn                     = NULL,
		.uri_match_fn                 = NULL
	};

	esp_log_level_set(TAG, ESP_LOG_DEBUG);
	esp_log_level_set("httpd_uri", ESP_LOG_ERROR); /* Annoying with not_found warnings */

	err = httpd_start(&httpd_handle, &config);
	if (err) {
		ESP_LOGE(TAG, "Failed to start server: %s", err2str(err));
		return;
	}

	ESP_LOGI(TAG, "Server started!");
	http_server_cmd_register(httpd_handle, &cmd_config);
	httpd_register_uri_handler(httpd_handle, &get_res_uri);
	httpd_register_err_handler(httpd_handle, HTTPD_404_NOT_FOUND, handle_web_requests);
	ESP_LOGD(TAG, "Handler registered!");
}
