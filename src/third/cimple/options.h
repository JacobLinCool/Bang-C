/**
 * @file options.h
 * @author JacobLinCool <hi@jacoblin.cool> (github.com/JacobLinCool)
 * @brief The options header file of Cimple Lib.
 * @version
 * @date
 *
 * @copyright Copyright (c) 2022 JacobLinCool (MIT License)
 * @see https://github.com/JacobLinCool/Cimple-Lib
 */
#ifndef __CIMPLE_UTILS_OPTIONS_H
#define __CIMPLE_UTILS_OPTIONS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct ParsedOption {
    char* name;
    char* value;
} ParsedOption;

typedef struct ParsedOptions {
    size_t        option_count;
    ParsedOption* options;
    size_t        wild_count;
    char**        wilds;
} ParsedOptions;

ParsedOptions* __parse_options(size_t argc, char* argv[]) {
    ParsedOptions* options = malloc(sizeof(ParsedOptions));
    options->option_count = 0;
    options->options = malloc(sizeof(ParsedOption) * argc);
    options->wild_count = 0;
    options->wilds = malloc(sizeof(char*) * argc);

    for (size_t i = 1; i < argc; i++) {
        char* arg = argv[i];
        if (arg[0] == '-') {
            char* name;
            if (arg[1] == '-') {
                name = strdup(arg + 2);
            } else {
                name = strdup(arg + 1);
            }

            char* value = NULL;
            char* equal = strchr(name, '=');
            if (equal) {
                *equal = '\0';
                value = strdup(equal + 1);
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                value = strdup(argv[i + 1]);
                i++;
            }

            options->options[options->option_count].name = name;
            options->options[options->option_count].value = value;
            options->option_count++;
        } else {
            options->wilds[options->wild_count++] = strdup(arg);
        }
    }

    options->options = realloc(options->options, sizeof(ParsedOption) * options->option_count);
    options->wilds = realloc(options->wilds, sizeof(char*) * options->wild_count);
    return options;
}

char* __get_option(ParsedOptions* options, char* name) {
    for (size_t i = 0; i < options->option_count; i++) {
        if (strcmp(options->options[i].name, name) == 0) {
            return options->options[i].value;
        }
    }
    return NULL;
}

bool __has_option(ParsedOptions* options, char* name) {
    for (size_t i = 0; i < options->option_count; i++) {
        if (strcmp(options->options[i].name, name) == 0) {
            return true;
        }
    }
    return false;
}

void __free_options(ParsedOptions* options) {
    for (size_t i = 0; i < options->option_count; i++) {
        free(options->options[i].name);
        if (options->options[i].value) {
            free(options->options[i].value);
        }
    }
    free(options->options);

    for (size_t i = 0; i < options->wild_count; i++) {
        free(options->wilds[i]);
    }
    free(options->wilds);

    free(options);
}

bool __options_key_only(ParsedOptions* options, char* name) {
    for (size_t i = 0; i < options->option_count; i++) {
        if (strcmp(options->options[i].name, name) == 0) {
            if (options->options[i].value) {
                options->wild_count++;
                options->wilds = realloc(options->wilds, sizeof(char*) * options->wild_count);
                options->wilds[options->wild_count - 1] = options->options[i].value;
                options->options[i].value = NULL;
                return true;
            }
        }
    }
    return false;
}

struct {
    /**
     * @brief Parse command line arguments
     * @param argc number of arguments
     * @param argv array of arguments
     * @return parsed options
     */
    ParsedOptions* (*parse)(size_t argc, char* argv[]);
    /**
     * @brief Get an option from the parsed options
     * @param options parsed options
     * @param name name of the option
     * @return value of the option
     */
    char* (*get)(ParsedOptions* options, char* name);
    /**
     * @brief Check if an option is present
     * @param options parsed options
     * @param name name of the option
     * @return true if the option is present, false otherwise
     */
    bool (*has)(ParsedOptions* options, char* name);
    /**
     * @brief Free the parsed options
     * @param options parsed options
     */
    void (*free)(ParsedOptions* options);
    /**
     * @brief Fix the option to be key only, will move the value to the wilds
     * @param options parsed options
     * @param name name of the option
     * @return true if the option is fixed, false otherwise
     */
    bool (*key_only)(ParsedOptions* options, char* name);
} Options = {
    .parse = __parse_options,
    .get = __get_option,
    .has = __has_option,
    .free = __free_options,
    .key_only = __options_key_only,
};

#endif  // __CIMPLE_UTILS_OPTIONS_H
