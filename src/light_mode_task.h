/**
 * @file light_mode_tasks.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-04-08
 *
 * @brief
 */

#ifndef LUMOS_LIGHT_MODE_TASK_H
#define LUMOS_LIGHT_MODE_TASK_H

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

#include <stdint.h>
#include <stdbool.h>

struct light_mode_task_queue_data {
	bool                   shutdown;
	uint8_t                mode;
	struct light_mode_data *mode_data;
};

/**
 * @brief Callback functions for this module
 *
 */
struct light_mode_task_callbacks {
	/**
	 * @brief Callback function.
	 *
	 * Called every time a light mode ends.
	 *
	 */
	void (*on_mode_task_end)(uint8_t mode);
};

void light_mode_task_start(uint8_t mode, struct light_mode_data *data);
void light_mode_task_stop(void);
void light_mode_task_register_callbacks(const struct light_mode_task_callbacks *cbks);
int light_mode_task_init(void);

#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* ifndef LUMOS_LIGHT_MODE_TASK_H */
