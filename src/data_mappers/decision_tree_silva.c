/**
 * Data mapper for a decision tree to filesystem.
 *
 * @file decision_tree_silva.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "decision_tree_silva.h"

#include <stdlib.h>
#include <string.h>


/** Size of the reading buffer. */
#define BUFFER_SIZE 32


/** Stringifier - part 2. */
#define STR2(x) #x

/** Stringifier - part 1. */
#define STR(X) STR2(X)


/***********************************************************************
 * Internal functions.
 **********************************************************************/

/**
 * Parses a node or subtree.
 *
 * @param[out] N Node
 * @param[in,out] stream Stream
 * @param[in] n_labels Number of labels
 */
static void parse_node(
    DecisionTreeNode *N,
    FILE *stream,
    const unsigned int n_labels
);



/**
 * Parses a leaf.
 *
 * @param[out] N Node
 * @param[in,out] stream Stream
 * @param[in] n_labels Number of labels
 */
static void parse_leaf(
    DecisionTreeNode *N,
    FILE *stream,
    const unsigned int n_labels
) {
    unsigned int i, n, *scores = (unsigned int *) malloc(n_labels * sizeof(unsigned int));

    if (scores == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    for (i = 0; i < n_labels; ++i) {
        n = fscanf(stream, "%u", scores + i);
        if (n != 1) {
            fprintf(stderr, "[%s: %d] Cannot parse leaf.\n", __FILE__, __LINE__);
            abort();
        }
    }

    decision_tree_leaf_create(N, scores, n_labels);
}



/**
 * Parses a leaf with logarithmic distribution of probabilities.
 *
 * @param[out] N Node
 * @param[in,out] stream Stream
 * @param[in] n_labels Number of labels
 */
static void parse_leaf_logarithmic(
    DecisionTreeNode *N,
    FILE *stream,
    const unsigned int n_labels
) {
    unsigned int i, n;
    double *scores = (double *) malloc(n_labels * sizeof(double));

    if (scores == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    for (i = 0; i < n_labels; ++i) {
        n = fscanf(stream, "%lg", scores + i);
        if (n != 1) {
            fprintf(stderr, "[%s: %d] Cannot parse leaf.\n", __FILE__, __LINE__);
            abort();
        }
    }

    decision_tree_leaf_logarithmic_create(N, scores, n_labels, 1.0);
}



/**
 * Parses an univariate linear split.
 *
 * @param[out] N Node
 * @param[in,out] stream Stream
 * @param[in] n_labels Number of labels
 */
static void parse_univariate_linear_split(
    DecisionTreeNode *N,
    FILE *stream,
    const unsigned int n_labels
) {
    unsigned int n, feature;
    double threshold;
    DecisionTreeNode L, R;

    n = fscanf(stream, "%u %lf", &feature, &threshold);
    if (n != 2) {
        fprintf(stderr, "[%s: %d] Cannot parse univariate linear split.\n", __FILE__, __LINE__);
        abort();
    }

    parse_node(&L, stream, n_labels);
    parse_node(&R, stream, n_labels);

    decision_tree_univariate_linear_split_create(N, feature, threshold);
    decision_tree_univariate_linear_split_set_left_child(*N, L);
    decision_tree_univariate_linear_split_set_right_child(*N, R);
}



static void parse_node(
    DecisionTreeNode *N,
    FILE *stream,
    const unsigned int n_labels
) {
    unsigned int n;
    char node_type[BUFFER_SIZE];

    n = fscanf(stream, "%" STR(BUFFER_SIZE) "s", node_type);
    if (n != 1) {
        fprintf(stderr, "[%s: %d] Cannot parse decision tree node.\n", __FILE__, __LINE__);
        abort();
    }

    if (strcmp(node_type, "LEAF") == 0) {
        parse_leaf(N, stream, n_labels);
    }

    else if (strcmp(node_type, "LEAF_LOGARITHMIC") == 0) {
        parse_leaf_logarithmic(N, stream, n_labels);
    }

    else if (strcmp(node_type, "SPLIT") == 0) {
        parse_univariate_linear_split(N, stream, n_labels);
    }

    else {
        fprintf(stderr, "[%s: %d] Cannot parse decision tree node.\n", __FILE__, __LINE__);
        abort();
    }
}



/***********************************************************************
 * Public functions.
 **********************************************************************/

void decision_tree_silva_read(DecisionTree *T, FILE *stream) {
    unsigned int n, i, space_size, n_labels;
    char **labels, buffer[BUFFER_SIZE];
    BinaryTree root;

    if (!stream) {
        fprintf(stderr, "[%s: %d] Cannot read file.\n", __FILE__, __LINE__);
        abort();
    }

    /* Parses header. */
    n = fscanf(stream, "%" STR(BUFFER_SIZE) "s", buffer);
    if (n != 1 || strcmp(buffer, "classifier-decision-tree") != 0) {
        fprintf(stderr, "[%s: %d] Cannot parse decision tree.\n", __FILE__, __LINE__);
        abort();
    }

    /* Parses feature space size and number of labels */
    n = fscanf(stream, "%u %u", &space_size, &n_labels);
    if (n != 2) {
        fprintf(stderr, "[%s: %d] Cannot parse decision tree.\n", __FILE__, __LINE__);
        abort();
    }

    /* Parses labels */
    labels = (char **) malloc(n_labels * sizeof(char *));
    for (i = 0; i < n_labels; ++i) {
        labels[i] = (char *) malloc(BUFFER_SIZE * sizeof(char));
        n = fscanf(stream, "%" STR(BUFFER_SIZE) "s", labels[i]);
        if (n != 1) {
            fprintf(stderr, "[%s: %d] Cannot parse decision tree.\n", __FILE__, __LINE__);
            abort();
        }
    }

    /* Parses decision tree */
    parse_node(&root, stream, n_labels);


    /* Builds decision tree */
    decision_tree_create(T, root, space_size, labels, n_labels);
}
