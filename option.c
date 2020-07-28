#include "option.h"

static void trim(char *string) {
    char buffer[OPTION_VALUE_SIZE];
    char *trimmer = string;
    while (*trimmer == ' ' || *trimmer == '\n') {
        ++trimmer;
    }
    strcpy(buffer, trimmer);
    trimmer = buffer + strlen(buffer);
    while (*trimmer == '\0' || *trimmer == ' ') {
        --trimmer;
    }
    *(trimmer + 1) = '\0';
    strcpy(string, buffer);
}

void option_read(Option *option, FILE *stream) {
    unsigned int n;

    n = fscanf(stream, "%[^:]:", option->name);
    if (n != 1) {
        strcpy(option->name, "");
        strcpy(option->value, "");
        return;
    }
    trim(option->name);

    fgets(option->value, OPTION_VALUE_SIZE, stream);
    if (option->value[strlen(option->value) - 1] == '\n') {
        option->value[strlen(option->value) - 1] = '\0';
    }
    trim(option->value);
}


void option_print(const Option option, FILE *stream) {
    fprintf(stream, "%s: %s\n", option.name, option.value);
}
