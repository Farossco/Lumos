#pragma once

#include <stdbool.h>
#include <esp_err.h>

#define SPIFFS_READ_CB_STOP     false
#define SPIFFS_READ_CB_CONTINUE true

typedef bool (*spiffs_read_cb_t)(char *chunk_data, size_t chunk_size, void *ag);

esp_err_t spiffs_read_file(char *buf, size_t buf_size, const char *path);

esp_err_t spiffs_read_file_chunks(char *chunk_buf, size_t buf_size, const char *path,
                                     spiffs_read_cb_t on_chunk_read_cb, void *arg);

esp_err_t spiffs_init(void);
