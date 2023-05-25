/**
 * @file alarm_item.c
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-16
 *
 * @brief
 */

#include "alarm_item.h"
#include <sys/queue.h>
#include <esp_log.h>

static const char *TAG = "alarm_item_dynamic";

struct alarm_item {
	struct alarm_data data;

	LIST_ENTRY(alarm_item) le;
};

static LIST_HEAD( , alarm_item) list;
static uint16_t item_count;

struct alarm_data * alarm_item_data_ptr_get(struct alarm_item *item)
{
	assert(item);

	return &item->data;
}

struct alarm_item * alarm_item_get_from_id(alarm_id_t id)
{
	struct alarm_item *item;

	LIST_FOREACH(item, &list, le) {
		if (item->data.id == id) {
			return item;
		}
	}

	return NULL;
}

struct alarm_item * alarm_item_create(void)
{
	struct alarm_item *item;

	if (item_count >= CONFIG_ALARM_ITEM_MAX) {
		ESP_LOGE(TAG, "Cannot create more item! Consider increasing CONFIG_ALARM_ITEM_MAX");
		return NULL;
	}

	item = malloc(sizeof(struct alarm_item));
	if (!item) {
		ESP_LOGE(TAG, "Failed to allocate memory!");
		return NULL;
	}

	LIST_INSERT_HEAD(&list, item, le);

	item_count++;

	return item;
}

struct alarm_item * alarm_item_get_next(struct alarm_item *item)
{
	if (item == NULL) {
		return LIST_FIRST(&list);
	} else {
		return LIST_NEXT(item, le);
	}
}

void alarm_item_rm(struct alarm_item *item)
{
	assert(item);

	assert(item_count); /* Item count cannot be 0 if we are trying to remove an item */

	LIST_REMOVE(item, le);

	item_count--;

	free(item);
}

void alarm_item_clr(void)
{
	struct alarm_item *item, *titem;

	LIST_FOREACH_SAFE(item, &list, le, titem) {
		free(item);
	}

	LIST_INIT(&list);

	item_count = 0;
}
