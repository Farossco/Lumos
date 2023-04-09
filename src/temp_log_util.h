#undef ESP_LOGE
#undef ESP_LOGW
#undef ESP_LOGD
#undef ESP_LOGV
#undef ESP_LOGI
#undef ESP_LOG_BUFFER_HEX

#define ESP_LOGE(TAG, ...)                        printf("[%s] [ERR] ", TAG); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGW(TAG, ...)                        printf("[%s] [WRN] ", TAG); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGD(TAG, ...)                        printf("[%s] [DBG] ", TAG); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGV(TAG, ...)                        printf("[%s] [VER] ", TAG); printf(__VA_ARGS__); printf("\n")
#define ESP_LOGI(TAG, ...)                        printf("[%s] [INF] ", TAG); printf(__VA_ARGS__); printf("\n")
#define ESP_LOG_BUFFER_HEX(tag, buffer, buff_len) esp_log_buffer_hex_internal(tag, buffer, buff_len, ESP_LOG_INFO)
