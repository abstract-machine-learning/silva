/**
 * Data mapper for a classifier in silva format.
 *
 * @file classifier_silva.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "classifier_silva.h"

#include <stdlib.h>
#include <string.h>

#include "decision_tree_silva.h"
#include "forest_silva.h"


/** Size of buffer. */
#define BUFFER_SIZE 32


/** Stringifier - part 2. */
#define STR2(x) #x

/** Stringifier - part 1. */
#define STR(X) STR2(X)


void classifier_silva_read(Classifier *C, FILE *stream) {
    unsigned int n;
    char classifier_type[BUFFER_SIZE];

    if (!stream) {
        fprintf(stderr, "[%s: %d] Cannot read file.\n", __FILE__, __LINE__);
        abort();
    }

    n = fscanf(stream, "%" STR(BUFFER_SIZE) "s", classifier_type);
    if (n != 1) {
        fprintf(stderr, "[%s: %d] Cannot parse classifier.\n", __FILE__, __LINE__);
        abort();
    }
    rewind(stream);

    if (strcmp(classifier_type, "classifier-decision-tree") == 0) {
        DecisionTree T;
        decision_tree_silva_read(&T, stream);
        classifier_create_decision_tree(C, T);
    }
    else if (strcmp(classifier_type, "classifier-forest") == 0) {
        Forest F;
        forest_silva_read(&F, stream);
        classifier_create_forest(C, F);
    }
    else {
        fprintf(stderr, "[%s: %d] Unsupported type of classifier.\n", __FILE__, __LINE__);
        abort();
    }
}
