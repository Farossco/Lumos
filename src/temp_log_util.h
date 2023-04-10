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
