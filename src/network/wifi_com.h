#ifndef NETWORK_WIFI_COM_H
#define NETWORK_WIFI_COM_H

struct wifi_com_conn_callbacks {
	void (*on_connected)(void);
};

void wifi_com_init(void);

void wifi_com_register_conn_callbacks(const struct wifi_com_conn_callbacks *callbacks);

#endif /* ifndef NETWORK_WIFI_COM_H */
