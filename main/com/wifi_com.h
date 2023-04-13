#ifndef NETWORK_WIFI_COM_H
#define NETWORK_WIFI_COM_H

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

#include <esp_err.h>

struct wifi_com_conn_callbacks {
	void (*on_connected)(void);
};

esp_err_t wifi_com_init(void);

void wifi_com_register_conn_callbacks(const struct wifi_com_conn_callbacks *callbacks);

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* ifndef NETWORK_WIFI_COM_H */
