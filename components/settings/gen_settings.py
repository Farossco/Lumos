# coding=utf-8
import argparse
import csv
from packaging import version
import re, os


def remove_comments_from_csv(csv_file):
    """
    This function takes a CSV file and removes all comments from each row of the file.
    """
    # Iterate through each row in the CSV file
    for row in csv_file:
        # Split the row by "#" and take the first element
        raw = row.split("#")[0].strip()
        # If the result is not an empty string, yield it
        if raw:
            yield raw


def parse_input_groups(input_groups_path, length_max):
    """
    This function reads a CSV file and extracts information about groups of configuration settings.
    It returns the version number of the configuration file, and a list of dictionaries containing
    the name and type of each group.
    """
    group_list = []
    group_version = None

    # Open the input_groups CSV file and remove comments
    try:
        with open(input_groups_path) as csv_file:
            csv_reader = csv.reader(remove_comments_from_csv(csv_file))

            # Iterate through each row in the CSV file
            for row in csv_reader:
                try:
                    # Check if the row specifies a version number
                    if row[0].strip().lower().startswith("@v"):
                        group_version = version.parse(row[0][2:])
                        continue

                    # Create a dictionary for the current group
                    group_item = dict()

                    # Extract the name of the group and convert to lowercase
                    group_item["name"] = row[0].lower()

                    # Check that the length of the group name is within the allowed maximum
                    if len(group_item["name"]) > length_max:
                        raise Exception(
                            f"Length of '{group_item['name']}' ({len(group_item['name'])}) "
                            f"exceeds maximum of {str(length_max)}. "
                            "Consider increasing CONFIG_SETTINGS_GROUP_LENGTH_MAX"
                        )

                    # Determine the type of the group
                    if len(row) == 1:
                        group_item["type"] = "SINGLE"
                    else:
                        group_item["type"] = row[1].strip().upper()

                    # If the group is of type "MULTI", extract the selector key
                    if group_item["type"] == "MULTI":
                        try:
                            group_item["selector"] = row[2].strip().lower()
                        except IndexError:
                            raise Exception(
                                f"No selector key specified for multi "
                                "instances group: '{group_item['name']}'"
                            )
                    elif group_item["type"] != "SINGLE":
                        raise Exception("Unknown group type " + group_item["type"])

                    # Add the group to the list
                    group_list.append(group_item)
                except Exception as e:
                    raise Exception(
                        f"Group parsing error ! {e}. Bad entry: \"{','.join(row)}\""
                    )
    except FileNotFoundError:
        raise FileNotFoundError("Configuration file not found: " + input_groups_path)

    # Check that a version number was provided
    if not group_version:
        raise Exception(
            "Groups: No version provided - "
            f"Use @Vx.x.x in {os.path.basename(input_groups_path)} "
            f"to define the version"
        )

    # Return the version number and list of groups
    return group_version, group_list


def parse_input_keys(input_keys_path, groups_list, length_max):
    keys_list = []
    keys_version = None

    try:
        with open(input_keys_path) as csv_file:
            csv_reader = csv.reader(remove_comments_from_csv(csv_file))

            deprecated = False

            for row in csv_reader:
                try:
                    if row[0].strip().lower().startswith("@v"):
                        keys_version = version.parse(row[0][2:])
                        continue
                    elif row[0].strip().lower() == "@deprecated":
                        # deprecate the next item
                        deprecated = True
                        continue

                    key_item = dict()

                    key_item["group"] = row[0].strip().lower()

                    if deprecated:
                        deprecated = False
                        key_item["deprecated"] = True
                        key_item["name"] = ""
                        key_item["type"] = ""
                        key_item["default"] = ""
                        keys_list.append(key_item)
                        continue

                    key_item["deprecated"] = False

                    groups_exists = False
                    for group_item in groups_list:
                        if group_item["name"] == key_item["group"]:
                            groups_exists = True

                    if not groups_exists:
                        raise Exception(f"Group \"{key_item['group']}' doesn't exists")

                    name = row[2].strip().lower()

                    # Check that the key is not already in the list
                    for checker_key_item in keys_list:
                        if checker_key_item["name"] == name:
                            raise Exception(
                                f"Key '{checker_key_item['name']}' already exists"
                            )

                    key_item["type"] = row[1].strip().lower()
                    key_item["name"] = name
                    if len(key_item["name"]) > length_max:
                        raise Exception(
                            f"Length of '{group_item['name']}' ({len(group_item['name'])}) "
                            f"exceeds maximum of {str(length_max)}. "
                            "Consider increasing CONFIG_SETTINGS_GROUP_LENGTH_MAX"
                        )

                    try:
                        key_item["default"] = ""
                        for i, sub_default in enumerate(row[3:]):
                            if i != 0:
                                key_item["default"] += ", "
                            key_item["default"] += sub_default.strip()
                    except IndexError:
                        raise Exception(
                            f"No default value provided for {key_item['name']}"
                        )

                    keys_list.append(key_item)

                except Exception as e:
                    raise Exception(
                        f"Key parsingg error ! {e}. Bad entry: \"{','.join(row)}\""
                    )
            if deprecated:
                raise Exception(
                    'Tag "Deprecated" was specified but there is no key to apply it to'
                )
    except FileNotFoundError:
        raise FileNotFoundError(f"Configuration not found: {input_keys_path}")

    # Check that a version number was provided
    if not keys_version:
        raise Exception(
            "Keys: No version provided - "
            f"Use @Vx.x.x in {os.path.basename(input_keys_path)} "
            f"to define the version"
        )

    return keys_version, keys_list


def generate(
    version,
    group_list,
    key_list,
    in_defs_path,
    out_header_path,
    out_header_internal_path,
):
    with open(out_header_path, "w+") as out_header:
        out_header.write("#pragma once\n\n")

        try:
            try:
                with open(in_defs_path) as in_defs:
                    out_header.write(in_defs.read())
                    out_header.write("\n\n")
            except FileNotFoundError:
                raise FileNotFoundError(f"Definition file not found: {in_defs_path}")

            out_header.write("enum {\n")

            first_row = True
            for index, key_item in enumerate(key_list):
                if key_item["deprecated"]:
                    continue

                row = ""

                if first_row:
                    first_row = False
                else:
                    row += ",\n"

                row += "\tSETTINGS_"
                row += str(key_item["name"]).upper()
                row += " = "
                row += str(index)

                out_header.write(row)

            out_header.write("\n" "};\n\n")

            out_header.write("enum {\n")

            for index, group_item in enumerate(group_list):
                row = ""
                if index != 0:
                    row += ",\n"

                row += "\tSETTINGS_GROUP_"
                row += str(group_item["name"]).upper()
                row += " = "
                row += str(index)

                out_header.write(row)

            out_header.write("\n};\n")
        except Exception as e:
            out_header.write(
                "\n#error \"Got Python error '{e}' while generating this file\"\n"
            )
            raise Exception(e)

    with open(out_header_internal_path, "w+") as out_header_internal:
        try:
            out_header_internal.write(
                f"#pragma once\n"
                "\n"
                f"#include <{os.path.basename(out_header_path)}>\n"
                "\n"
                "/* Settings version */\n"
                f"#define SETTINGS_VERSION_MAJOR {version.major}\n"
                f"#define SETTINGS_VERSION_MINOR {version.minor}\n"
                f"#define SETTINGS_VERSION_REV {version.micro}\n"
                "\n"
                "/* Settings data */\n"
                "#define SETTINGS_DATA"
            )

            for key_item in key_list:
                if key_item["deprecated"]:
                    continue
                match = re.fullmatch(
                    "([0-9a-zA-Z_]*)(\[[0-9a-zA-Z_]*\])", key_item["type"]
                )
                if match:
                    out_header_internal.write(
                        f" \\\n\t{match.group(1)} {key_item['name']} {match.group(2)};"
                    )
                else:
                    out_header_internal.write(
                        f" \\\n\t{key_item['type']} {key_item['name']};"
                    )

            out_header_internal.write(
                "\n\n/* Settings data default values */\n#define SETTINGS_DATA_DEFAULTS {"
            )

            first_item = True
            for key_item in key_list:
                if key_item["deprecated"]:
                    continue
                if first_item:
                    first_item = False
                else:
                    out_header_internal.write(", ")
                out_header_internal.write(key_item["default"])

            out_header_internal.write(
                "}\n\n"
                "/* Settings groups full names */\n"
                "#define SETTINGS_GROUP_FULL_NAMES"
            )

            for group_item in group_list:
                if group_item["type"] == "MULTI":
                    out_header_internal.write(
                        f" \\\n\tSETTINGS_FULL_NAME({group_item['name']});"
                    )

            out_header_internal.write(
                "\n\n/* Settings key entries */\n#define SETTINGS_KEYS"
            )

            for key_item in key_list:
                out_header_internal.write(" \\\n\tSETTINGS_KEY_ENTRY")
                if key_item["deprecated"]:
                    out_header_internal.write("_DEPRECATED,")
                else:
                    out_header_internal.write(
                        f"({key_item['name']}, SETTINGS_GROUP_{key_item['group'].upper()}),"
                    )
            out_header_internal.write("\n\n")

            # Settings group entries
            out_header_internal.write(
                "/* Settings group entries */\n#define SETTINGS_GROUPS"
            )

            for group_item in group_list:
                out_header_internal.write(" \\\n\tSETTINGS_GROUP_ENTRY")
                if group_item["type"] == "MULTI":
                    out_header_internal.write("_MULTI")
                out_header_internal.write("(" + group_item["name"])
                if group_item["type"] == "MULTI":
                    out_header_internal.write(
                        ", " + "SETTINGS_" + group_item["selector"].upper()
                    )
                out_header_internal.write("),")
            out_header_internal.write("\n")
        except Exception as e:
            out_header_internal.write(
                "\n#error \"Got Python error '{e}' while generating this file\"\n"
            )
            raise Exception(e)


def check_versions(groups_version, keys_version):
    if groups_version != keys_version:
        raise Exception(
            f"Versions mismatch! 'keys' is V{keys_version} and 'groups' is V{groups_version}"
        )

    return groups_version


def check_selectors(group_list, key_list):
    for index, group_item in enumerate(group_list):
        if group_item["type"] != "MULTI":
            continue

        selector_exists = False
        for key_item in key_list:
            if key_item["name"] == group_item["selector"]:
                selector_exists = True
                if key_item["group"] == group_item["name"]:
                    raise Exception(
                        f"The provided selector '{group_item['selector']}' for the group "
                        f"'{group_item['name']}' is in the group '{key_item['group']}'. "
                        f"This makes no sense..."
                    )
                if key_item["type"] != "settings_group_id_t":
                    raise Exception(
                        f"The provided selector '{group_item['selector']}' for the group "
                        f"'{group_item['name']}' has incorrect type '{key_item['type']}'. "
                        f"Selector type must be 'settings_group_id_t'"
                    )
                if key_item["deprecated"]:
                    raise Exception(
                        f"The provided selector '{group_item['selector']}' for the group "
                        f"'{group_item['name']}' is deprecated, this is not allowed. "
                        "If necessary, remove the group and deprecate all of its keys"
                    )
                group_of_selector_before_current = False
                for test_group_item in group_list[:index]:
                    if key_item["group"] == test_group_item["name"]:
                        group_of_selector_before_current = True
                        break
                if not group_of_selector_before_current:
                    raise Exception(
                        f"The provided selector '{group_item['selector']}' for the group "
                        f"'{group_item['name']}' is in the group '{key_item['group']}'."
                        f"Thus, the group '{key_item['group']}' must be declared before "
                        f"'{group_item['name']}' in the group list"
                    )
                break
        if not selector_exists:
            raise Exception(
                f"The provided selector '{group_item['selector']}' for the group "
                f"'{group_item['name']}' doesn't exist"
            )


def check_empty_groups(groups_list, keys_list):
    for group_item in groups_list:
        one_key_present = False
        deprecated_present = False
        for key_item in keys_list:
            if key_item["group"] == group_item["name"]:
                if key_item["deprecated"]:
                    deprecated_present = True
                else:
                    one_key_present = True
                    break

        if not one_key_present:
            if deprecated_present:
                raise Exception(
                    f"Group '{group_item['name']}' only contains deprecated keys, add keys to it or remove the group"
                )
            else:
                raise Exception(f"Group '{group_item['name']}' is empty")


def parse_args():
    """Parse arguments"""
    global args
    parser = argparse.ArgumentParser(
        description="Settings library resources generator",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    group = parser.add_argument_group("input arguments")
    group.add_argument(
        "--input_keys",
        required=True,
        metavar="FILE",
        help="Input CSV file path for keys",
    )
    group.add_argument(
        "--input_groups",
        required=True,
        metavar="FILE",
        help="Input CSV file path for groups",
    )
    group.add_argument(
        "--input_defs",
        required=True,
        metavar="FILE",
        help="Input header file path for additional definitions",
    )

    group = parser.add_argument_group("output arguments")
    group.add_argument(
        "--header",
        type=argparse.FileType("w"),
        metavar="FILE",
        help="Output header file path for definitions",
    )
    group.add_argument(
        "--header_internal",
        type=argparse.FileType("w"),
        metavar="FILE",
        help="Output header file path for internal definitions",
    )
    group.add_argument(
        "--config_key_length_max",
        required=True,
        help="Configuration: Key maximum length",
    )
    group.add_argument(
        "--config_group_length_max",
        required=True,
        help="Configuration: Group maximum length",
    )

    args = parser.parse_args()


if __name__ == "__main__":
    parse_args()

    try:
        groups_version, groups = parse_input_groups(
            args.input_groups, int(args.config_group_length_max)
        )
        keys_version, keys = parse_input_keys(
            args.input_keys, groups, int(args.config_key_length_max)
        )
        version = check_versions(groups_version, keys_version)
        check_selectors(groups, keys)
        check_empty_groups(groups, keys)
        generate(
            version,
            groups,
            keys,
            args.input_defs,
            args.header.name,
            args.header_internal.name,
        )
    except Exception as exc:
        print("[SETTINGS] Error: " + str(exc))
        exit(1)
