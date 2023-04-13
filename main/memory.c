#include "memory.h"
#include <esp_spiffs.h>
#include <nvs_flash.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "utils.h"
#include "kconfig_stub.h"

const char *TAG = "memory";

static SemaphoreHandle_t memory_fs_sem;

static nvs_handle_t memory_nvs_handle;
static SemaphoreHandle_t memory_nvs_mutex;

esp_err_t memory_fs_read_file(char *buf, size_t buf_size, const char *path)
{
	FILE *file;
	size_t read_size;
	char full_path[255];
	struct stat sbuf;
	esp_err_t ret = 0;

	if (path[0] == '/') {
		snprintf(full_path, sizeof(full_path), "/spiffs%s", path);
	} else {
		snprintf(full_path, sizeof(full_path), "/spiffs/%s", path);
	}

	if (xSemaphoreTake(memory_fs_sem, CONFIG_MEM_FS_MUTEX_TIMEOUT_MS) == pdFALSE) {
		ESP_LOGE(TAG, "FS semaphore timed_out");
		return -EAGAIN;
	}

	/* Check if the requested file exists */
	if (stat(full_path, &sbuf)) {
		ESP_LOGE(TAG, "File %s doesn't exist or an error occured: %s", full_path, err2str(errno));
		ret = errno;
	} else {
		file = fopen(full_path, "r");
		if (file == NULL) {
			ESP_LOGE(TAG, "Failed to open %s: %s", full_path, err2str(errno));
			ret = errno;
		} else {
			read_size = fread(buf, 1, buf_size, file);
			if (read_size == 0 && ferror(file) != 0) {
				ESP_LOGE(TAG, "Failed to read file %s! %s", full_path, err2str(errno));
				ret = errno;
			}

			fclose(file);
		}
	}

	if (xSemaphoreGive(memory_fs_sem) == pdFALSE) {
		ESP_LOGE(TAG, "Failed to release semaphore");
		return -ENOMEM;
	}

	return ret;
}

esp_err_t memory_fs_read_file_chunks(char *chunk_buf, size_t buf_size, const char *path,
                                     memory_fs_read_cb_t on_chunk_read_cb, void *arg)
{
	FILE *file;
	size_t read_size;
	char full_path[255];
	struct stat sbuf;
	esp_err_t ret = 0;

	if (on_chunk_read_cb == NULL) {
		return -ESP_ERR_INVALID_ARG;
	}

	if (path[0] == '/') {
		snprintf(full_path, sizeof(full_path), "/spiffs%s", path);
	} else {
		snprintf(full_path, sizeof(full_path), "/spiffs/%s", path);
	}

	if (xSemaphoreTake(memory_fs_sem, CONFIG_MEM_FS_MUTEX_TIMEOUT_MS) == pdFALSE) {
		ESP_LOGE(TAG, "FS semaphore timed_out");
		return -EAGAIN;
	}

	/* Check if the requested file exists */
	if (stat(full_path, &sbuf)) {
		ESP_LOGE(TAG, "File %s doesn't exist or an error occured: %s", full_path, err2str(errno));
		ret = errno;
	} else {
		file = fopen(full_path, "r");
		if (file == NULL) {
			ESP_LOGE(TAG, "Failed to open %s: %s", full_path, err2str(errno));
			ret = errno;
		} else {
			do{
				read_size = fread(chunk_buf, 1, buf_size, file);
				if (read_size == 0 && ferror(file) != 0) {
					ESP_LOGE(TAG, "Failed to read file %s! %s", full_path, err2str(errno));
					ret = errno;
				} else {
					if (on_chunk_read_cb(chunk_buf, read_size, arg) == MEMORY_FS_READ_CB_STOP) {
						ESP_LOGD(TAG, "File read stopped by user");
						ret = -ESP_ERR_NOT_FINISHED;
						break;
					}
				}
			}while (read_size > 0);

			fclose(file);
		}
	}

	if (xSemaphoreGive(memory_fs_sem) == pdFALSE) {
		ESP_LOGE(TAG, "Failed to release semaphore");
		return -ENOMEM;
	}

	return ret;
}

char * memory_nvs_get_str(const char *key, char *buf, size_t buf_size)
{
	esp_err_t err;
	size_t length;
	char *out_buf = NULL;

	if (xSemaphoreTake(memory_nvs_mutex, CONFIG_MEM_FS_MUTEX_TIMEOUT_MS) == pdFALSE) {
		ESP_LOGE(TAG, "NVS mutex timed_out");
		return NULL;
	}

	err = nvs_get_str(memory_nvs_handle, key, NULL, &length);
	if (err) {
		ESP_LOGW(TAG, "Could not get NVS string entry for %s: %s", key, err2str(err));
	} else {
		if (length >= buf_size) { /* Accouting for '\0' */
			ESP_LOGW(TAG, "Provided buffer too small, (is %d, should be %d at least)", buf_size, length + 1);
		} else {
			err = nvs_get_str(memory_nvs_handle, key, buf, &length);
			if (err) {
				ESP_LOGE(TAG, "Could not get NVS string entry on second attempt for %s: %s", key, err2str(err));
			} else {
				out_buf = buf;
			}
		}
	}

	if (xSemaphoreGive(memory_nvs_mutex) == pdFALSE) {
		ESP_LOGE(TAG, "Failed to release NVS mutex");
		return NULL;
	}

	return out_buf;
}

esp_err_t memory_nvs_set_str(const char *key, const char *str)
{
	esp_err_t err;
	esp_err_t ret = 0;

	if (xSemaphoreTake(memory_nvs_mutex, CONFIG_MEM_FS_MUTEX_TIMEOUT_MS) == pdFALSE) {
		ESP_LOGE(TAG, "NVS mutex timed_out");
		return -EAGAIN;
	}

	err = nvs_set_str(memory_nvs_handle, key, str);
	if (err) {
		ESP_LOGE(TAG, "Could not set NVS string for %s: %s", key, err2str(err));
	} else {
		ret = err;
	}

	err = nvs_commit(memory_nvs_handle);
	if (err) {
		printf("Failed to commit for string %s: %s", key, err2str(err));
		return err;
	}

	if (xSemaphoreGive(memory_nvs_mutex) == pdFALSE) {
		ESP_LOGE(TAG, "Failed to release NVS mutex");
		return -EAGAIN;
	}

	return ret;
}

static esp_err_t memory_fs_init(void)
{
	esp_err_t err;

	ESP_LOGI(TAG, "Initializing fs");

	esp_vfs_spiffs_conf_t conf = {
		.base_path              = "/spiffs",
		.partition_label        = NULL,
		.max_files              = CONFIG_MEM_FS_MAX_FILE,
		.format_if_mount_failed = false
	};

	err = esp_vfs_spiffs_register(&conf);
	if (err != ESP_OK) {
		if (err == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount filesystem");
		} else if (err == ESP_ERR_NOT_FOUND) {
			ESP_LOGE(TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(err));
		}
		return err;
	}

	memory_fs_sem = xSemaphoreCreateCounting(CONFIG_MEM_FS_MAX_FILE, CONFIG_MEM_FS_MAX_FILE);
	if (memory_fs_sem == NULL) {
		ESP_LOGE(TAG, "Failed to create fs semaphore: %s", err2str(-ENOMEM));
		return -ENOMEM;
	}

	return 0;
}

static esp_err_t memory_nvs_init(void)
{
	esp_err_t err;

	ESP_LOGI(TAG, "Initializing nvs");

	err = nvs_flash_init();
	if (err) {
		ESP_LOGE(TAG, "Failed to initialize nvs_flash: %s", err2str(err));
		return err;
	}

	err = nvs_open("lumos", NVS_READWRITE, &memory_nvs_handle);
	if (err != ESP_OK) {
		printf("Failed to open nvs: %s", err2str(err));
		return err;
	}

	memory_nvs_mutex = xSemaphoreCreateMutex();
	if (memory_nvs_mutex == NULL) {
		ESP_LOGE(TAG, "Failed to create nvs mutex: %s", err2str(-ENOMEM));
		return -ENOMEM;
	}

	return 0;
}

esp_err_t memory_init(void)
{
	esp_err_t err;

	ESP_LOGI(TAG, "Initializing memory");

	err = memory_fs_init();
	if (err) {
		ESP_LOGE(TAG, "Failed to initialize spiffs: %s", err2str(err));
		return err;
	}

	err = memory_nvs_init();
	if (err) {
		ESP_LOGE(TAG, "Failed to initialize nvs");
		return err;
	}

	return 0;
}
