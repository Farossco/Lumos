#pragma once

#include <esp_err.h>
#include <alarm.h>

#define ALARM_TYPE_WAKEUP 0
#define ALARM_TYPE_DONTEXIST 1

esp_err_t alarms_init();
