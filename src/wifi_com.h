#ifndef WIFI_COM_H
#define WIFI_COM_H

/**
 * @brief wifi_com connexion callbacks
 *
 */
struct wifi_com_conn_callbacks {
	void (*on_connected)(void);
};

/**
 * @brief Initialize wifi communication
 *
 */
void wifi_com_init();

/**
 * @brief Register wifi_com connexion callbacks
 *
 * @param callbacks Callback structure pointer
 */
void wifi_com_register_conn_callbacks(const struct wifi_com_conn_callbacks *callbacks);

#endif /* ifndef WIFI_COM_H */
