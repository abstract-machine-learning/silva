#ifndef OPTION_H
#define OPTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPTION_NAME_SIZE 64
#define OPTION_VALUE_SIZE 128

typedef struct option Option;

struct option {
    char name[OPTION_NAME_SIZE];
    char value[OPTION_VALUE_SIZE];
};


void option_read(Option *option, FILE *stream);
void option_print(const Option option, FILE *stream);

#endif
