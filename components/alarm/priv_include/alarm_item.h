/**
 * @file alarm_item.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-16
 *
 * @brief
 */

#pragma once

#include <time.h>
#include <stdbool.h>
#include "alarm_priv_types.h"

struct alarm_item;

struct alarm_item * alarm_item_get_from_id(alarm_id_t id);
struct alarm_item * alarm_item_get_next(struct alarm_item *item);
struct alarm_item * alarm_item_create(void);
struct alarm_data * alarm_item_data_ptr_get(struct alarm_item *item);
void alarm_item_rm(struct alarm_item *item);
void alarm_item_clr(void);
