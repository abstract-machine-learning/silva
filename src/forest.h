/**
 * Defines a forest.
 *
 * A forest is a set of trees which cooperates to solve a
 * classification or regression problem.
 *
 * @file forest.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef FOREST_H
#define FOREST_H

#include <stdio.h>

#include "decision_tree.h"
#include "set.h"


/** Type of a forest. */
typedef struct forest *Forest;


/** Types of voting scheme. */
typedef enum {
    FOREST_VOTING_MAX,        /**< Labels are computed as the ones getting
                                   more votes. */
    FOREST_VOTING_AVERAGE,    /**< Labels are computed as the ones having the
                                   maximal average probability. */
    FOREST_VOTING_SOFTARGMAX  /**< Labels are computed as the ones having
                                   the maximal probability, obtained as
                                   \f$\frac{e^{score_i}}{\sum_{j \in \mathcal{L}} e^{score_j}}\f$,
                                   which is also (incorrectly) called softmax. */
} ForestVotingScheme;



/**
 * Creates a forest.
 *
 * @param[out] F Pointer to forest to create
 * @param[in] n_trees Maximum number of trees
 * @param[in] voting_scheme Voting scheme
 * @warning #forest_delete should be called to ensure proper memory
 *          deallocation.
 */
void forest_create(
    Forest *F,
    const unsigned int n_trees,
    const ForestVotingScheme voting_scheme
);


/**
 * Deletes a forest.
 *
 * Every tree in the forest is deleted as well.
 *
 * @param[out] F Pointer to forest to delete
 * @warning Trees in the forest are deleted.
 */
void forest_delete(Forest *F);



/**
 * Returns size of feature space of a forest.
 *
 * @param[in] F Forest
 * @return Size of feature space
 */
unsigned int forest_get_feature_space_size(const Forest F);


/**
 * Returns labels in a forest.
 *
 * @param[in] F Forest
 * @return Labels in the forest
 */
char **forest_get_labels_as_array(const Forest F);


/**
 * Returns number of labels in a forest.
 *
 * @param[in] F Forest
 * @return Number of labels
 */
unsigned int forest_get_n_labels(const Forest F);


/**
 * Returns voting scheme of a forest.
 *
 * @param[in] F Forest
 * @return Voting scheme
 */
ForestVotingScheme forest_get_voting_scheme(const Forest F);


/**
 * Returns number of trees in the forest.
 *
 * @param[in] F Forest
 * @return Number of trees in the forest
 */
unsigned int forest_get_n_trees(const Forest F);


/**
 * Returns tree in a forest as an array.
 *
 * @param[in] F Forest
 * @return Array of tress
 */
DecisionTree *forest_get_trees_as_array(const Forest F);



/**
 * Sets voting scheme.
 *
 * @param[in,out] F Forest
 * @param[in] voting_scheme Voting scheme
 */
void forest_set_voting_scheme(
    Forest F,
    const ForestVotingScheme voting_scheme
);


/**
 * Computes probabilities of classes of a sample.
 *
 * @param[out] scores Array of scores for given sample
 * @param[in] F Forest
 * @param[in] x Sample
 * @note Scores depend depends on the voting scheme set for the forest.
 *       See #forest_set_voting_scheme to change the voting scheme.
 */
void forest_compute_decision_function(
    double *scores,
    const Forest F,
    const double *x
);


/**
 * Classifies a sample.
 *
 * @param[out] labels #Set of winning labels
 * @param[in] F Forest
 * @param[in] x Sample
 */
void forest_classify(
    Set labels,
    const Forest F,
    const double *x
);


/**
 * Prints a forest.
 *
 * @param[in] F Forest
 * @param[out] stream Stream
 */
void forest_print(const Forest F, FILE *stream);

#endif
