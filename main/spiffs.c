#include "spiffs.h"
#include <esp_spiffs.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "utils.h"
#include "kconfig_stub.h"

static const char *TAG = "spiffs";

static SemaphoreHandle_t spiffs_sem;

esp_err_t spiffs_read_file(char *buf, size_t buf_size, const char *path)
{
	FILE *file;
	size_t read_size;
	char full_path[255];
	struct stat sbuf;
	esp_err_t ret = ESP_OK;

	if (path[0] == '/') {
		snprintf(full_path, sizeof(full_path), "/spiffs%s", path);
	} else {
		snprintf(full_path, sizeof(full_path), "/spiffs/%s", path);
	}

	if (xSemaphoreTake(spiffs_sem, CONFIG_MEM_FS_MUTEX_TIMEOUT_MS) == pdFALSE) {
		ESP_LOGE(TAG, "Couldn't take semaphore");
		return EAGAIN;
	}

	/* Check if the requested file exists */
	if (stat(full_path, &sbuf)) {
		ESP_LOGE(TAG, "File %s doesn't exist or an error occured: %s", full_path, err2str(errno));
		ret = errno;
	} else if (buf) {
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

	if (xSemaphoreGive(spiffs_sem) == pdFALSE) {
		ESP_LOGE(TAG, "Failed to release semaphore");
		return ESP_ERR_INVALID_STATE;
	}

	return ret;
}

esp_err_t spiffs_read_file_chunks(char *chunk_buf, size_t buf_size, const char *path,
                                     spiffs_read_cb_t on_chunk_read_cb, void *arg)
{
	FILE *file;
	size_t read_size;
	char full_path[255];
	struct stat sbuf;
	esp_err_t ret = ESP_OK;

	if (on_chunk_read_cb == NULL) {
		return ESP_ERR_INVALID_ARG;
	}

	if (path[0] == '/') {
		snprintf(full_path, sizeof(full_path), "/spiffs%s", path);
	} else {
		snprintf(full_path, sizeof(full_path), "/spiffs/%s", path);
	}

	if (xSemaphoreTake(spiffs_sem, CONFIG_MEM_FS_MUTEX_TIMEOUT_MS) == pdFALSE) {
		ESP_LOGE(TAG, "Couldn't take semaphore");
		return EAGAIN;
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
					if (on_chunk_read_cb(chunk_buf, read_size, arg) == SPIFFS_READ_CB_STOP) {
						ESP_LOGD(TAG, "File read stopped by user");
						ret = -ESP_ERR_NOT_FINISHED;
						break;
					}
				}
			}while (read_size > 0);

			fclose(file);
		}
	}

	if (xSemaphoreGive(spiffs_sem) == pdFALSE) {
		ESP_LOGE(TAG, "Failed to release semaphore");
		return ESP_ERR_INVALID_STATE;
	}

	return ret;
}

esp_err_t spiffs_init(void)
{
	esp_err_t err;

	ESP_LOGI(TAG, "Initializing SPIFFS");

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

	spiffs_sem = xSemaphoreCreateCounting(CONFIG_MEM_FS_MAX_FILE, CONFIG_MEM_FS_MAX_FILE);
	if (spiffs_sem == NULL) {
		ESP_LOGE(TAG, "Failed to create fs semaphore: %s", err2str(ESP_ERR_NO_MEM));
		return ESP_ERR_NO_MEM;
	}

	return ESP_OK;
}
