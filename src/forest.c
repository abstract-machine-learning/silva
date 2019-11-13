/**
 * Implements a forest.
 *
 * A forest is a set of trees which cooperates to solve a
 * classification or regression problem.
 *
 * @file forest.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */

#include "forest.h"

#include <stdlib.h>
#include <string.h>


/** Structure of a random forest. */
struct forest {
    ForestVotingScheme voting_scheme;  /**< Voting scheme. */
    DecisionTree *trees;       /**< Aray of trees. */
    unsigned int n_trees;      /**< Maximum number of trees in the forest. */
};



/***********************************************************************
 * Internal functions.
 **********************************************************************/

/**
 * Computes decision function using the MAX voting scheme.
 *
 * @param[out] scores Resulting votes
 * @param[in] F Forest
 * @param[in] x Sample
 */
static void decision_function_max(
    double *scores,
    const Forest F,
    const double * const x
) {
    const unsigned int n_labels = forest_get_n_labels(F);
    unsigned int i;
    double *tree_scores = malloc(n_labels * sizeof(double));

    if (tree_scores == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    /* Initializes scores to 0 for each label */
    for (i = 0; i < n_labels; ++i) {
        scores[i] = 0.0;
    }

    /* Computes scores for each tree */
    for (i = 0; i < F->n_trees; ++i) {
        unsigned int j;
        double max;

        decision_tree_compute_decision_function(tree_scores, F->trees[i], x);

        /* Finds maximum score */
        max = tree_scores[0];
        for (j = 1; j < n_labels; ++j) {
            if (tree_scores[j] > max ) {
                max = tree_scores[j];
            }
        }

        /* Assigns one vote to each label having maximum score */
        for (j = 0; j < n_labels; ++j) {
            if (tree_scores[j] == max) {
                scores[j] += 1.0;
            }
        }
    }

    /* Deallocates memory */
    free(tree_scores);
}



/**
 * Computes decision function using the AVERAGE voting scheme.
 *
 * @param[out] scores Resulting votes
 * @param[in] F Forest
 * @param[in] x Sample
 */
static void decision_function_average(
    double *scores,
    const Forest F,
    const double * const x
) {
    const unsigned int n_labels = forest_get_n_labels(F);
    unsigned int i, j;
    double *tree_scores = malloc(n_labels * sizeof(double));

    if (tree_scores == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    /* Initializes scores to 0 for each label */
    for (i = 0; i < n_labels; ++i) {
        scores[i] = 0.0;
    }

    /* Computes scores for each tree */
    for (i = 0; i < F->n_trees; ++i) {
        decision_tree_compute_decision_function(tree_scores, F->trees[i], x);

        /* Updates average score */
        for (j = 0; j < n_labels; ++j) {
            scores[j] += tree_scores[j] / (double) F->n_trees;
        }
    }

    /* Deallocates memory */
    free(tree_scores);
}



/**
 * Computes decision function using the SOFTARGMAX voting scheme.
 *
 * @param[out] scores Resulting votes
 * @param[in] F Forest
 * @param[in] x Sample
 */
static void decision_function_softargmax(
    double *scores,
    const Forest F,
    const double * const x
) {
    const unsigned int n_labels = forest_get_n_labels(F);
    unsigned int i, j;
    double *tree_scores = malloc(n_labels * sizeof(double));

    if (tree_scores == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    /* Initializes scores to 0 for each label */
    for (i = 0; i < n_labels; ++i) {
        scores[i] = 0.0;
    }

    /* Computes scores for each tree */
    for (i = 0; i < F->n_trees; ++i) {
        decision_tree_compute_decision_function(tree_scores, F->trees[i], x);

        /* Updates average score */
        for (j = 0; j < n_labels; ++j) {
            scores[j] += tree_scores[j];
        }
    }

    /* Deallocates memory */
    free(tree_scores);
}





/***********************************************************************
 * Public functions.
 **********************************************************************/
void forest_create(
    Forest *F,
    const unsigned int n_trees,
    const ForestVotingScheme voting_scheme
) {
    Forest f = (Forest) malloc(sizeof(struct forest));
    if (f == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    f->trees = (DecisionTree *) malloc(n_trees * sizeof(DecisionTree));
    if (f->trees == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }
    f->n_trees = n_trees;
    f->voting_scheme = voting_scheme;

    *F = f;
}



void forest_delete(Forest *F) {
    unsigned int i;

    if (F == NULL || *F == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    for (i = 0; i < (*F)->n_trees; ++i) {
        decision_tree_delete((*F)->trees + i);
    }

    free((*F)->trees);
    free(*F);
    *F = NULL;
}



unsigned int forest_get_feature_space_size(const Forest F) {
    if (F == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return decision_tree_get_feature_space_size(F->trees[0]);
}



char **forest_get_labels_as_array(const Forest F) {
    if (F == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return decision_tree_get_labels_as_array(F->trees[0]);
}



unsigned int forest_get_n_labels(const Forest F) {
    if (F == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return decision_tree_get_n_labels(F->trees[0]);
}



ForestVotingScheme forest_get_voting_scheme(const Forest F) {
    if (F == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return F->voting_scheme;
}



unsigned int forest_get_n_trees(const Forest F) {
    if (F == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return F->n_trees;
}



DecisionTree *forest_get_trees_as_array(const Forest F) {
    if (F == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return F->trees;
}



void forest_set_voting_scheme(
    Forest F,
    const ForestVotingScheme voting_scheme
) {
    if (F == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    F->voting_scheme = voting_scheme;
}



void forest_compute_decision_function(
    double *scores,
    const Forest F,
    const double *x
) {
    if (F == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    switch (F->voting_scheme) {
        case FOREST_VOTING_MAX:
            decision_function_max(scores, F, x);
            break;

        case FOREST_VOTING_AVERAGE:
            decision_function_average(scores, F, x);
            break;

        case FOREST_VOTING_SOFTARGMAX:
            decision_function_softargmax(scores, F, x);
            break;

    }
}



void forest_classify(
    Set labels,
    const Forest F,
    const double *x
) {
    const unsigned int n_labels = forest_get_n_labels(F);
    char ** const labels_array = forest_get_labels_as_array(F);
    unsigned int i;
    double max,
           *scores = (double *) malloc(n_labels * sizeof(double));

    set_clear(labels);
    forest_compute_decision_function(scores, F, x);

    max = scores[0];
    for (i = 1; i < n_labels; ++i) {
        if (scores[i] > max) {
            max = scores[i];
        }
    }

    for (i = 0; i < n_labels; ++i) {
        if (scores[i] == max) {
            set_add_element(labels, labels_array[i]);
        }
    }

    free(scores);
}



void forest_print(const Forest F, FILE *stream) {
    if (F == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    unsigned int i;

    fprintf(
        stream,
        "Forest @%p, contains %u trees, voting scheme: ",
        (void *) F,
        F->n_trees
    );

    switch (F->voting_scheme) {
        case FOREST_VOTING_MAX:
            fprintf(stream, "maximum consensus");
            break;

        case FOREST_VOTING_AVERAGE:
            fprintf(stream, "maximum of average of probabilities");
            break;

        case FOREST_VOTING_SOFTARGMAX:
            fprintf(stream, "softargmax algorithm");
            break;
    }
    fprintf(stream, "\n");

    for (i = 0; i < F->n_trees; ++i) {
        decision_tree_print(F->trees[i], stream);
    }
}
