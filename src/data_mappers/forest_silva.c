/**
 * Data mapper for a random forest to file system.
 *
 * @file forest_silva.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "forest_silva.h"

#include <stdlib.h>
#include <string.h>

#include "decision_tree_silva.h"


/** Size of read buffer. */
#define BUFFER_SIZE 32


/** Stringifier - part 2. */
#define STR2(x) #x

/** Stringifier - part 1. */
#define STR(x) STR2(x)


void forest_silva_read(
    Forest *F,
    FILE *stream
) {
    unsigned int n_trees, i, n;
    char buffer[BUFFER_SIZE];
    DecisionTree *trees;

    if (!stream) {
        fprintf(stderr, "[%s: %d] Cannot read file.\n", __FILE__, __LINE__);
        abort();
    }

    n = fscanf(stream, "%" STR(BUFFER_SIZE) "s %u", buffer, &n_trees);
    if (n != 2 || strcmp(buffer, "classifier-forest") != 0) {
        fprintf(stderr, "[%s: %d] Cannot parse random forest.\n", __FILE__, __LINE__);
        abort();
    }

    forest_create(F, n_trees, FOREST_VOTING_MAX);
    trees = forest_get_trees_as_array(*F);
    for (i = 0; i < n_trees; ++i) {
        decision_tree_silva_read(trees + i, stream);
    }
}
