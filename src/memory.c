#include "memory.h"
#include <esp_spiffs.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include "temp_log_util.h"
#include "utils_c.h"

const char *TAG = "memory";

esp_err_t memory_fs_read_file(char *buf, size_t buf_size, const char *path)
{
	FILE *file;
	size_t read_size;
	char full_path[255];
	struct stat sbuf;
	int err;

	if (path[0] == '/') {
		snprintf(full_path, sizeof(full_path), "/spiffs%s", path);
	} else {
		snprintf(full_path, sizeof(full_path), "/spiffs/%s", path);
	}

	/* Check if the requested file exists */
	if (stat(full_path, &sbuf)) {
		ESP_LOGE(TAG, "File %s doesn't exist or an error occured: %s", full_path, err2str(errno));
		return errno;
	}

	file = fopen(full_path, "r");
	if (file == NULL) {
		ESP_LOGE(TAG, "Failed to open %s: %s", full_path, err2str(errno));
		return errno;
	}

	read_size = fread(buf, 1, buf_size, file);
	if (read_size == 0 && ferror(file) != 0) {
		ESP_LOGE(TAG, "Failed to read file %s! %s", full_path, err2str(errno));
		fclose(file);
		return errno;
	}

	fclose(file);

	return 0;
}

esp_err_t memory_fs_read_file_chunks(char *chunk_buf, size_t buf_size, const char *path,
                                     memory_fs_read_cb_t on_chunk_read_cb, void *arg)
{
	FILE *file;
	size_t read_size;
	char full_path[255];
	struct stat sbuf;
	int err;

	if (on_chunk_read_cb == NULL) {
		return -ESP_ERR_INVALID_ARG;
	}

	if (path[0] == '/') {
		snprintf(full_path, sizeof(full_path), "/spiffs%s", path);
	} else {
		snprintf(full_path, sizeof(full_path), "/spiffs/%s", path);
	}

	/* Check if the requested file exists */
	if (stat(full_path, &sbuf)) {
		ESP_LOGE(TAG, "File %s doesn't exist or an error occured: %s", full_path, err2str(errno));
		return errno;
	}

	file = fopen(full_path, "r");
	if (file == NULL) {
		ESP_LOGE(TAG, "Failed to open %s: %s", full_path, err2str(errno));
		return errno;
	}

	do{
		read_size = fread(chunk_buf, 1, buf_size, file);
		if (read_size == 0 && ferror(file) != 0) {
			ESP_LOGE(TAG, "Failed to read file %s! %s", full_path, err2str(errno));
			fclose(file);
			return errno;
		}

		if (on_chunk_read_cb(chunk_buf, read_size, arg) == MEMORY_FS_READ_CB_STOP) {
			ESP_LOGD(TAG, "File read stopped by user");
			fclose(file);
			return -ESP_ERR_NOT_FINISHED;
		}
	}while(read_size > 0);

	fclose(file);

	return 0;
}

static esp_err_t memory_fs_init(void)
{
	esp_err_t err;

	esp_vfs_spiffs_conf_t conf = {
		.base_path              = "/spiffs",
		.partition_label        = NULL,
		.max_files              = 10,
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
}

esp_err_t memory_init(void)
{
	esp_err_t err;

	err = memory_fs_init();
	if (err) {
		ESP_LOGE(TAG, "Failed to initialize spiffs: %d", err);
		return err;
	}
}
