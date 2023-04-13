#ifndef UTILS_C_H
#define UTILS_C_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#undef ESP_LOGE
#undef ESP_LOGW
#undef ESP_LOGI
#undef ESP_LOGD
#undef ESP_LOGV

#define ESP_LOGE(TAG, ...) printf("[%s][%d] [ERR] ", TAG, __LINE__); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGW(TAG, ...) printf("[%s][%d] [WRN] ", TAG, __LINE__); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGI(TAG, ...) printf("[%s][%d] [INF] ", TAG, __LINE__); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGD(TAG, ...) printf("[%s][%d] [DBG] ", TAG, __LINE__); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGV(TAG, ...) printf("[%s][%d] [VER] ", TAG, __LINE__); printf(__VA_ARGS__); printf("\n")

#define ARRAY_SIZE(x)      (sizeof(x) / sizeof(*x))
#define RGB_BLACK (rgb_t) { 0 }
#define RGB_RED   (rgb_t) { { 255 }, { 0 }, { 0 } }
#define RGB_GREEN (rgb_t) { { 0 }, { 255 }, { 0 } }
#define RGB_BLUE  (rgb_t) { { 0 }, { 0 }, { 255 } }
#define RGB_WHITE (rgb_t) { { 255 }, { 255 }, { 255 } }

extern char err_buf[50];

#define err2str(err) (esp_err_to_name_r(err, err_buf, sizeof(err_buf)))

static inline bool str_endswith(const char *str, const char *pattern)
{
	size_t pattern_length = strlen(pattern);

	return strlen(str) >= pattern_length && (strcmp(str + strlen(str) - pattern_length, pattern) == 0);
}

#endif /* ifndef UTILS_C_H */