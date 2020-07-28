#include "configuration.h"
#include "stdlib.h"

#define CONFIGURATION_INITIAL_CAPACITY 32

static void configuration_resize(Configuration *configuration) {
    configuration->options = (Option *) realloc(configuration->options, 2 * configuration->capacity * sizeof(Option));
    configuration->capacity *= 2;
}

static void configuration_add_option(Configuration *configuration, const Option option) {
    if (configuration->size == configuration->capacity) {
        configuration_resize(configuration);
    }
    memcpy(configuration->options + configuration->size, &option, sizeof(Option));
    ++configuration->size;
}

void configuration_create(Configuration *configuration) {
    configuration->options = (Option *) malloc(CONFIGURATION_INITIAL_CAPACITY * sizeof(Option));
    configuration->size = 0;
    configuration->capacity = CONFIGURATION_INITIAL_CAPACITY;
}


void configuration_delete(Configuration configuration) {
    free(configuration.options);
}


char *configuration_get_value(const Configuration configuration, const char *name) {
    unsigned int i;
    for (i = 0; i < configuration.size; ++i) {
        if (strcmp(name, configuration.options[i].name) == 0) {
            return configuration.options[i].value;
        }
    }
    return NULL;
}


void configuration_read(Configuration *configuration, FILE *stream) {
    Option option;
    while (!feof(stream)) {
        option_read(&option, stream);
        if (strlen(option.name) > 0) {
        configuration_add_option(configuration, option);
        }
    }
}


void configuration_print(const Configuration configuration, FILE *stream) {
    unsigned int i;
    for (i = 0; i < configuration.size; ++i) {
        option_print(configuration.options[i], stream);
    }
}
