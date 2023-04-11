#ifndef JSON_H
#define JSON_H

#include "Types.h"
#include <stdbool.h>

esp_err_t json_get_data_pretty(char *buf, size_t size);
esp_err_t json_get_data(char *buf, size_t size);
esp_err_t json_get_resources_pretty(char *buf, size_t size);
esp_err_t json_get_resources(char *buf, size_t size);
esp_err_t json_get_error_pretty(char *buf, size_t size, RequestError error);
esp_err_t json_get_error(char *buf, size_t size, RequestError error);

#endif // ifndef JSON_H
