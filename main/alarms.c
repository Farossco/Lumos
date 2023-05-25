#include "alarms.h"
#include <esp_log.h>
#include "json.h"
#include "utils.h"

static const char *TAG = "alarms";

static bool on_json_gen_list(alarm_id_t id, alarm_type_id_t type_id, bool active,
                             struct alarm_schedule schedule, const void *json_ctx)
{
	json_gen_cb_ctx_t obj_main_ctx;

	JSON_GEN_ADD_OBJECT(json_ctx, "alarm", &obj_main_ctx);
	JSON_GEN_ADD_INT(&obj_main_ctx, "Id",      id);
	JSON_GEN_ADD_INT(&obj_main_ctx, "Type id", type_id);
	JSON_GEN_ADD_BOOL(&obj_main_ctx, "Active", active);
	{
		json_gen_cb_ctx_t obj_sch_ctx;
		JSON_GEN_ADD_OBJECT(&obj_main_ctx, "Schedule", &obj_sch_ctx);
		JSON_GEN_ADD_INT(&obj_sch_ctx, "hour", schedule.time.hour);
		JSON_GEN_ADD_INT(&obj_sch_ctx, "min",  schedule.time.min);
		JSON_GEN_ADD_INT(&obj_sch_ctx, "sec",  schedule.time.sec);
		{
			json_gen_cb_ctx_t array_sch_ao_ctx;
			JSON_GEN_ADD_ARRAY(&obj_sch_ctx, "active on", &array_sch_ao_ctx);
			JSON_GEN_ARRAY_ADD_BOOL(&array_sch_ao_ctx, schedule.active_on.mon);
			JSON_GEN_ARRAY_ADD_BOOL(&array_sch_ao_ctx, schedule.active_on.tue);
			JSON_GEN_ARRAY_ADD_BOOL(&array_sch_ao_ctx, schedule.active_on.wed);
			JSON_GEN_ARRAY_ADD_BOOL(&array_sch_ao_ctx, schedule.active_on.thu);
			JSON_GEN_ARRAY_ADD_BOOL(&array_sch_ao_ctx, schedule.active_on.fri);
			JSON_GEN_ARRAY_ADD_BOOL(&array_sch_ao_ctx, schedule.active_on.sat);
			JSON_GEN_ARRAY_ADD_BOOL(&array_sch_ao_ctx, schedule.active_on.sun);
		}
	}

	return ESP_OK;
}

static esp_err_t on_json_gen(const json_gen_cb_ctx_t *ctx, const void *user_data)
{
	return alarm_iterate(on_json_gen_list, ctx);
}

static struct json_gen_sub_data json_gen_sub_data = {
	.type_id   = JSON_TYPE_DATA,
	.name      = "Alarms",
	.cb        = on_json_gen,
	.user_data = NULL
};

esp_err_t alarms_init()
{
	esp_err_t err;

	err = alarm_init();
	if (err) {
		ESP_LOGE(TAG, "Failed to initialize alarm: %s", esp_err_to_name(err));
		return err;
	}

	err = json_gen_sub(&json_gen_sub_data);
	if (err) {
		ESP_LOGE(TAG, "Failed to subscribe to data json: %s", esp_err_to_name(err));
		return err;
	}

	return ESP_OK;
}
