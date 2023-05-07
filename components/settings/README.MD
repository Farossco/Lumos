# Settings library

## Overview

This library allows to manage settings in an application. The settings work with a system of groups and keys. Each key belongs to a specific group. Settings are accessed only by giving their keys. Groups are transparent in the application except for resetting all settings in a group.

The benefit of groups is that they can have multiple instances if they are declared as "multi" group. This means that each key of this group can have multiple values stored in persistent memory, one value for each instance. Each "multi" group have a selector key. This selector key itself belongs to another group, this is why you necessarily need to have at least one non "multi" group. It must also be an integer type. Its role is to select which instance of the group is loaded.

When the value of a selector is 0, the default instance is loaded. In this case, all setting have their default values and a change to a setting won't be kept in persistent memory. The value can still be modified but all changes would be lost at restart. There is no way to get the value for a given profile except the one that is currently loaded and this is all done implicitly.

Note that because the concept of group is completely abstracted in the application, two keys can never have the same name even if they are in two different groups.

## Example

In this example, let's declare two groups: a `non-multi` group called `base` and a `multi` group called `profile`.

The `base` group has two keys: `current_profile` and `current_country`. The `profile` group has several keys: `vib_intensity`, `led_profiles`, ...

Here, the selector for the `profile` group is the `current_profile` key. When loading the `profile` group, the value of `current_profile` is checked to load the selected instance.

More specifically, `profile` settings for user 1 are stored in `profile1/` but are accessed in the application independently of where they are stored. Accessing the `vibration_intensity` setting of the current user can be achieved by calling the `settings_get` function with the key `SETTINGS_VIBRATION_INTENSITY`.

## How it works

All settings value are loaded at run time and stored in the RAM. When a setting in written, its value is automatically writen on the flash. Reading a value results in reading the loaded value in RAM.

Settings are generated using csv files. Groups and keys must be defined in two separate files.

By default, keys must be defined in `res/settings_keys.csv` and groups in `res/settings_groups.csv`. These paths can be configured by respectively setting the Kconfig options `CONFIG_SETTINGS_PATH_KEYS` and `CONFIG_SETTINGS_PATH_GROUPS`.

You can add comments in the files by adding a '#' at the beginning.

### Defining a group

Each group must be defined on a separate line, the first value is the name
of the group (not case-sensitive) and the second value is the type of the
group. This defines if the group must contain multiple instances in flash
(this should be used for a profile group for example). In this case, the
type "multi" must be provided. If not, this can be left blank or be set at
"single". If a group is declared as "multi", you must also declare the
selector key for that group (in the example of a profile group, this could
be current_profile). This key must then be declared in whichever group in
the key declaration file. Note that a selector key can also belong to a
multi instance group. In this case, that group needs be declared prior to
the one with the selector.

Example

    # Setting version
    @V0.0.1, First version
    @V0.0.2, Deprecated vib_config

    # Name        Type        selector
    base
    profile,      MULTI,      current_profile

### Defining a key

Each key must belong to a group. When defining a key, you must first specify
its group. This this the first value in the line. Then, separated by a comma
is the type of the variable. This can be anything and is not subject to
verification. Then, comes the name of the key. Finally, an default value
must be provided. Its type must match the type of the key and this value is
also not subjected to verification.

Example

    # Setting version
    @V0.0.1, First version
    @V0.0.2, Deprecated vib_config

    # Group       Type                    Name                  Default value
    base,         uint8_t,                current_profile,      0

    profile,      pattern_profile_t,      vib_profiles,         DEFAULT_VIB_PROFILE
    profile,      pattern_profile_t,      led_profiles,         {1, 0, 0, 7, 4, 0}
    profile,      uint8_t,                vib_intensity,        0
    @Deprecated, Vibration configuration is now a fixed value.
    profile,      uint8_t,                vib_config,           0

### Additional definitions

All the additional types and macros must be defined in a particular header
file. By default, this file is looked for under res/settings_def.h. All the
content of this file is copied in the generated settings_def.h that is
included in settings.h.

Example

    #define DEFAULT_VIB_PROFILE {0, 1, 0, 1, 8, 0}

    typedef struct {
        uint8_t hazard_alert;
        uint8_t sleep_alert;
        uint8_t hazard_sent;
        uint8_t sequence_first;
        uint8_t error;
        uint8_t error_seq_timeout;
    } pattern_profile_t;

## Special tags

Special tags can be added at the beginning of a line to specify a
particular attribute either for the file or the following key.
Comments can be put after a comma to describe the reason behind this tag.

Special tags are not case-sensitive.

### Existing tags

| Tag           | Usable for keys | Usable for groups | Description                                                                                                                                       |
| -------------|----------------|-------------------|---------------------------------------------------------------------------------------------------------------------------------------------------|
| `@Vx.x.x`     | X              | X                 | Can be placed anywhere in the file to specify the version of that file. If written multiple times, only the last value will be used. This could be useful to write a log of all previous versions with information about this version as a comment. Note that both keys and group must provide a version number and errors will be thrown if at least one is not provided or if they don't match. |
| `@Deprecated` | X              |                   | This will mark the next encountered key as deprecated. Its value(s) will be removed from persistent memory on the first boot with that configuration. This should be used when a key is not needed anymore or if it needs to be modified. In this last case, a new key must then be created. This is crucial to preserve compatibility between connected devices. If all settings from a group have been deprecated, this group can simply be deleted. |

## About the ids

Setting keys are stored in 16-bit variables and their values are automatically defined from the csv file. The id of a key is equal to its position on the list, starting at 0. Since the settings can be written by a remote, once a setting has been defined, **IT MUST NEVER BE CHANGED OR REMOVED**! If a setting becomes obsolete or needs to be modified, it needs to be marked as `deprecated` and a new setting must be defined if necessary. This will ensure that the values of the keys won't shift and allow for an all-time compatibility. The version of the settings can be used to negotiate with a connected device and potentially ask for an update.

## Using the library

This library offers a very simple interface.

First, it must be initialized using the `settings_init` function. When this is done, all settings are loaded and group instances have been selected according to the value of their selector. To load a different instance, just modify the value of the selector and reload the library with the `settings_reload` function.

To access a setting, use `settings_get` and `settings_set` functions by giving the key of the setting, a pointer to the data to load from/to and the size of the data. Alternatively, use the `SETTINGS_GET` and `SETTINGS_SET` macros to abstract the size argument by using `sizeof()` on the data variable.

**Warning**: The size of the data must exactly match the size of the setting, otherwise, an error will be thrown and the setting won't be written/read.

To reset a single setting to its default value, simply use the `settings_reset` function or `SETTINGS_RESET` macro.

For synchronisation purposes, the version of the settings can be loaded using the `settings_version_get` function.

**Note**: The version is defined at compile time and cannot be changed.