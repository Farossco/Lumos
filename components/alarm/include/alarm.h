/**
 * @file alarm.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-13
 *
 * @brief
 */

#pragma once

#include <esp_err.h>
#include <sys/queue.h>
#include "alarm_types.h"

struct alarm_sub_data {
	alarm_type_id_t type_id;
	void            (*cb)(alarm_type_id_t type_id, alarm_id_t id);

	SLIST_ENTRY(alarm_sub_data) sle;
};

typedef bool (*alarm_iterate_cb)(alarm_id_t id, alarm_type_id_t type_id, bool active,
                                 struct alarm_schedule schedule, const void *user_data);

esp_err_t alarm_create(alarm_type_id_t type_id, struct alarm_schedule *schedule, bool persistent);

esp_err_t alarm_edit(alarm_id_t id, struct alarm_schedule *schedule);

esp_err_t alarm_delete(alarm_id_t id);

esp_err_t alarm_clear(void);

esp_err_t alarm_set_active(alarm_id_t id, bool active);

esp_err_t alarm_iterate(alarm_iterate_cb cb, const void *user_data);

esp_err_t alarm_sub(struct alarm_sub_data *sub_data);

esp_err_t alarm_init(void);
