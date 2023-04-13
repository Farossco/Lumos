#ifndef NETWORK_HTTP_SERVER_CMD_H
#define NETWORK_HTTP_SERVER_CMD_H

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

#include <esp_http_server.h>
#include <esp_err.h>
#include <stdint.h>

#define HTTP_SERVER_CMD_HANDLE(_cmd_type, _on_cmd_cb) \
	{                                                 \
		.cmd_type  = (_cmd_type),                     \
		.on_cmd_cb = (_on_cmd_cb),                    \
	}

/**
 * @brief HTTP server command callback type
 *
 * @param cmd_type The command type.
 * @param complement
 * @param value
 *
 * @return
 */
typedef esp_err_t (*http_server_cmd_cb)(const uint8_t *const cmd_type, uint32_t complement, uint32_t value);

/**
 * @brief HTTP server command callback type
 *
 * @param
 *
 * @return
 */
typedef esp_err_t (*http_server_cmd_end_cb)(httpd_req_t *rqst, const uint8_t error, char *error_str);

typedef struct {
	const uint8_t *const cmd_type;

	/**
	 * @brief Callback function.
	 *        Called on web command received
	 */
	http_server_cmd_cb on_cmd_cb;
} http_server_cmd_handle_t;

struct http_server_cmd_config {
	const http_server_cmd_handle_t *cmd_handles;
	size_t                         cmd_handles_size;

	http_server_cmd_cb             on_not_found_cb;

	http_server_cmd_end_cb         on_end_cb;
};

esp_err_t http_server_cmd_register(httpd_handle_t handle, const struct http_server_cmd_config *config);

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */


#endif /* ifndef NETWORK_HTTP_SERVER_H */
