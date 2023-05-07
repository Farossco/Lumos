/**
 * @file settings_internal.h
 * @author Farès Chati (faresticha@gmail.com)
 * @date 2023-05-07
 *
 * @brief
 */

#ifndef SETTINGS_INTERNAL_H
#define SETTINGS_INTERNAL_H

#include <stdbool.h>

/**
 * @brief Defined a setting key entry
 * 
 *        All settings key entries are generated during the project configuration
 * 
 */
#define SETTINGS_KEY_ENTRY(_var, _group_id) (struct settings_key_item) { \
		.group_item  = &settings_groups[_group_id],                      \
		.name        = #_var,                                            \
		.data_offset = offsetof(struct settings_data, _var),             \
		.data_size   = sizeof(((struct settings_data *)0)->_var)         \
}

/**
 * @brief This defines a deprecated setting key entry
 *
 *        A deprecated setting key must still be registered in the list but it will
 *        have no data in the settings_data structure. All values are set to 0 and the
 *        name of the key enum name also gets changed from SETTINGS_XXX to
 *        SETTINGS_DEPRECATED_XXX
 *
 *        All settings key entries are generated during the project configuration
 */
#define SETTINGS_KEY_ENTRY_DEPRECATED (struct settings_key_item) { \
		.group_item  = NULL,                                       \
		.name        = NULL,                                       \
		.data_offset = 0,                                          \
		.data_size   = 0                                           \
}

/**
 * @brief Defines a single-instance setting group entry
 *
 *        All settings group entries are generated during the project configuration
 */
#define SETTINGS_GROUP_ENTRY(_group) (struct settings_group_item) { \
		.name          = #_group,                                   \
		.selector_item = NULL,                                      \
		.full_name_ptr = NULL                                       \
}

/**
 * @brief Defines a multi-instance setting group entry.
 *
 *        All settings group entries are generated during the project configuration
 */
#define SETTINGS_GROUP_ENTRY_MULTI(_group, _s) (struct settings_group_item) { \
		.name          = #_group,                                             \
		.selector_item = &settings_keys[_s],                                  \
		.full_name_ptr = settings_groups_full_names._group                    \
}

/**
 * @brief Defines a full name string to generate full name for multi-instance groups.
 *
 *        This has the exact necessary size based on the maximum value number of instances for
 *        a group.
 * 
 *        All settings group entries are generated during the project configuration
 */
#define SETTINGS_FULL_NAME(_key) char _key[sizeof(#_key) + SETTINGS_INSTANCE_FULL_NAME_EXTRA_LENGTH]

/**
 * @brief Defines a version structure from the generated macros.
 */
#define SETTINGS_CURRENT_VERSION ((struct settings_version) { \
		.major = SETTINGS_VERSION_MAJOR,                      \
		.minor = SETTINGS_VERSION_MINOR,                      \
		.rev = SETTINGS_VERSION_REV })

/**
 * @brief Defines the number of extra characters in the group full name based on
 * the maximum number of instances
 *
 * The absolute maximum is set in the Kconfig to 65535
 */
#if CONFIG_SETTINGS_INSTANCE_MAX < 10
#define SETTINGS_INSTANCE_FULL_NAME_EXTRA_LENGTH 1
#elif CONFIG_SETTINGS_INSTANCE_MAX < 100
#define SETTINGS_INSTANCE_FULL_NAME_EXTRA_LENGTH 2
#elif CONFIG_SETTINGS_INSTANCE_MAX < 1000
#define SETTINGS_INSTANCE_FULL_NAME_EXTRA_LENGTH 3
#elif CONFIG_SETTINGS_INSTANCE_MAX < 10000
#define SETTINGS_INSTANCE_FULL_NAME_EXTRA_LENGTH 4
#else  /* if CONFIG_SETTINGS_INSTANCE_MAX < 10 */
#define SETTINGS_INSTANCE_FULL_NAME_EXTRA_LENGTH 5
#endif /* if CONFIG_SETTINGS_INSTANCE_MAX < 10 */

#endif /* ifndef SETTINGS_INTERNAL_H */
