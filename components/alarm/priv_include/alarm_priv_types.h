/**
 * @file alarm_priv_types.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-16
 *
 * @brief
 */

#pragma once

#include "alarm_types.h"

struct alarm_data {
	bool                  persistent;
	bool                  active;
	alarm_type_id_t       type_id;
	alarm_id_t            id;
	struct alarm_schedule schedule;
};
