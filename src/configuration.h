#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdio.h>
#include "option.h"

typedef struct configuration Configuration;

struct configuration {
    Option *options;
    unsigned int size;
    unsigned int capacity;
};

void configuration_create(Configuration *configuration);
void configuration_delete(Configuration configuration);
char *configuration_get_value(const Configuration configuration, const char *name);
void configuration_read(Configuration *configuration, FILE *stream);
void configuration_print(const Configuration configuration, FILE *stream);

#endif
