/**
 * @file alarm_types.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-16
 *
 * @brief
 */

#pragma once

#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <endian.h>

typedef uint8_t alarm_type_id_t;
typedef uint32_t alarm_id_t;

struct alarm_time {
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
};

inline time_t alarm_time_to_sec(struct alarm_time time)
{
	return time.hour * 3600 + time.min * 60 + time.sec;
}

inline struct alarm_time alarm_time_from_sec(time_t sec)
{
	return (struct alarm_time) {
			   .hour = sec / 3600,
			   .min  = (sec % 3600) / 60,
			   .sec  = sec % 60
	};
}

union alarm_dow {
	struct {
		/* Start the week with sunday to match struct tm wday */
#if BYTE_ORDER == LITTLE_ENDIAN
		uint8_t sun  : 1;
		uint8_t mon  : 1;
		uint8_t tue  : 1;
		uint8_t wed  : 1;
		uint8_t thu  : 1;
		uint8_t fri  : 1;
		uint8_t sat  : 1;
		uint8_t rsvd : 1;
#else /* if BYTE_ORDER == BIG_ENDIAN */
		uint8_t rsvd : 1;
		uint8_t sat  : 1;
		uint8_t fri  : 1;
		uint8_t thu  : 1;
		uint8_t wed  : 1;
		uint8_t tue  : 1;
		uint8_t mon  : 1;
		uint8_t sun  : 1;
#endif /* if BYTE_ORDER == BIG_ENDIAN */
	};
	uint8_t u8;
};

struct alarm_schedule {
	struct alarm_time time;
	union alarm_dow   active_on;
};
