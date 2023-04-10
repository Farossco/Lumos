#ifndef MEMORY_H
#define MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

#include <stdbool.h>
#include <esp_err.h>

#define MEMORY_FS_READ_CB_STOP     false
#define MEMORY_FS_READ_CB_CONTINUE true

#define MEMORY_NVS_KEY_WIFI_SSID   "W_SSID"
#define MEMORY_NVS_KEY_WIFI_PASS   "W_PASS"

#define MEMORY_NVS_GET_STR(key, buf) memory_nvs_get_str(MEMORY_NVS_KEY_ ## key, buf, sizeof(buf))
#define MEMORY_NVS_SET_STR(key, str) memory_nvs_set_str(MEMORY_NVS_KEY_ ## key, str)

typedef bool (*memory_fs_read_cb_t)(char *chunk_data, size_t chunk_size, void *ag);

esp_err_t memory_fs_read_file(char *buf, size_t buf_size, const char *path);

esp_err_t memory_fs_read_file_chunks(char *chunk_buf, size_t buf_size, const char *path,
                                     memory_fs_read_cb_t on_chunk_read_cb, void *arg);

char * memory_nvs_get_str(const char *key, char *buf, size_t buf_size);

esp_err_t memory_nvs_set_str(const char *key, char *str);

esp_err_t memory_init(void);

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* ifndef MEMORY_H */
