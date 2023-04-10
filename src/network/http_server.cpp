#include "http_server.h"
#include <esp_http_server.h>
#include "Request.h"
#include "Json.h"
#include "temp_log_util.h"
#include "kconfig_stub.h"
#include "memory.h"
#include "utils_c.h"

extern "C" {
static httpd_handle_t handle;

static const char *TAG = "http_server";

static bool on_chunk_read(char *chunk_data, size_t chunk_size, void *arg)
{
	httpd_req_t *rqst = (httpd_req_t *)arg;
	esp_err_t err;

	err = httpd_resp_send_chunk(rqst, chunk_data, chunk_size);
	if (err) {
		ESP_LOGE(TAG, "Failed to send chunk: %s", err2str(err));
		return MEMORY_FS_READ_CB_STOP;
	}

	return MEMORY_FS_READ_CB_CONTINUE;
}

static bool load_from_spiffs(char *query, httpd_req_t *rqst, String path)
{
	const char *data_type = "text/plain";
	char chunk_buf[1000];
	esp_err_t err;

	if (path.endsWith("/")) {
		path += "index.html";
	}

	if (httpd_query_key_value(query, "download", NULL, 0) == ESP_OK) {
		data_type = "application/octet-stream";
	} else if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
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

	httpd_resp_set_type(rqst, data_type);

	err = memory_fs_read_file_chunks(chunk_buf, sizeof(chunk_buf), path.c_str(), on_chunk_read, rqst);
	if (err) {
		ESP_LOGE(TAG, "Failed to read file %s: %s", path.c_str(), err2str(err));
		return false;
	}

	ESP_LOGV(TAG, "Response sent");

	return true;
}

static void request_display(httpd_req_t *rqst)
{
	ESP_LOGD(TAG, "Received request");

	ESP_LOGD(TAG, "Request: |%s|", rqst->uri);
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
	esp_err_t err;

	request_display(rqst);

	query_length = httpd_req_get_url_query_len(rqst);

	err = httpd_req_get_url_query_str(rqst, query, sizeof(query));
	if (err == ESP_ERR_NOT_FOUND) {
		query[0] = '\0';
	} else if (err) {
		ESP_LOGE(TAG, "Failed to get url query str: %s", err2str(err));
		return err;
	}

	if (!load_from_spiffs(query, rqst, rqst->uri)) {
		String message = "File Not Detected\n\n";

		message += String("URL: ") + rqst->uri + "\n";
		message += String("Method: ") + ((rqst->method == HTTP_GET) ? "GET" : "POST") + "\n";
		message += String("Query length: ") + query_length + "\n";
		message += String("Query: ") + query + "\n";

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

		err = httpd_resp_send(rqst, message.c_str(), message.length());
		if (err) {
			ESP_LOGE(TAG, "Failed to set status: %s", err2str(err));
			return err;
		}

		ESP_LOGV(TAG, "404 page sent");
	}

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

void http_server_start(void)
{
	esp_err_t err;

	const httpd_config_t config = {
		.task_priority                = tskIDLE_PRIORITY + 5,
		.stack_size                   = 8192,
		.core_id                      = CONFIG_NETWORK_CORE_ID,
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

	err = httpd_start(&handle, &config);
	if (err == ESP_OK) {
		ESP_LOGI(TAG, "Server started!");
		httpd_register_uri_handler(handle, &command_uri);
		httpd_register_uri_handler(handle, &get_res_uri);
		httpd_register_err_handler(handle, HTTPD_404_NOT_FOUND, handle_web_requests);
		ESP_LOGD(TAG, "Handler registered!");
	} else {
		ESP_LOGE(TAG, "Failed to start server: %s", err2str(err));
	}
}
}
