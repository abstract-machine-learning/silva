/**
 * Hyperrectangle analysis for decision trees.
 *
 * @file decision_tree_hyperrectangle.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "decision_tree_hyperrectangle.h"

#include <stdlib.h>

#include "../search_algorithms/depth_first.h"


/***********************************************************************
 * Internal functions and data structures.
 **********************************************************************/

/**
 * Computes labels associated to a score array.
 *
 * @param[out] labels #Set of labels
 * @param[in] scores Array of scores
 * @param[in] T #DecisionTree
 */
static void scores_to_labels(Set labels, const unsigned int *scores, const DecisionTree T) {
    const unsigned int n_labels = decision_tree_get_n_labels(T);
    unsigned int i, max;
    char **labels_array = decision_tree_get_labels_as_array(T);

    set_clear(labels);

    max = scores[0];
    for (i = 0; i < n_labels; ++i) {
        if (scores[i] > max) {
            max = scores[i];
        }
    }

    for (i = 0; i < n_labels; ++i) {
        if (scores[i] == max) {
            set_add_element(labels, labels_array[i]);
        }
    }
}



/**
 * Computes labels associated to a logarithmic score array.
 *
 * @param[out] labels #Set of labels
 * @param[in] scores Array of logarithmic scores
 * @param[in] T #DecisionTree
 */
static void log_scores_to_labels(Set labels, const double *scores, const DecisionTree T) {
    const unsigned int n_labels = decision_tree_get_n_labels(T);
    unsigned int i;
    double max;
    char **labels_array = decision_tree_get_labels_as_array(T);

    set_clear(labels);

    max = scores[0];
    for (i = 0; i < n_labels; ++i) {
        if (scores[i] > max) {
            max = scores[i];
        }
    }

    for (i = 0; i < n_labels; ++i) {
        if (scores[i] == max) {
            set_add_element(labels, labels_array[i]);
        }
    }
}



/** Structure of data of a counterexample searcher. */
struct counterexample_search_data {
    DecisionTree T;       /**< Decision tree. */
    Hyperrectangle x;     /**< #Hyperrectangle representing a region. */
    Set concrete_labels;  /**< #Set of concrete labels. */
    Set abstract_labels;  /**< #Set of abstract labels. */
};


/**
 * Tells whether a node is leaf containing a counterexample.
 *
 * @param[in] node #Node to test
 * @param[in] context Additional data
 * @return 1 if node contains a counterexample, 0 otherwise
 */
static unsigned int is_counterexample_leaf(const Node node, Context context) {
    struct counterexample_search_data *data = (struct counterexample_search_data *) context;

    switch (decision_tree_node_get_type(node)) {
    case DECISION_TREE_LEAF:
        scores_to_labels(data->abstract_labels, decision_tree_leaf_get_scores(node), data->T);
        break;

    case DECISION_TREE_LEAF_LOG:
        log_scores_to_labels(data->abstract_labels, decision_tree_leaf_logarithmic_get_scores(node), data->T);
        break;

    case DECISION_TREE_UNIVARIATE_LINEAR_SPLIT:
        return 0;
    }

    return !set_is_equal(data->concrete_labels, data->abstract_labels);
}



/**
 * Computes a list of reachable nodes.
 *
 * @param[out] L #List of reachable #Node
 * @param[in] node #Node to examine
 * @param[in] context Additional data
 */
static void compute_reachable_paths(List L, const Node node, Context context) {
    struct counterexample_search_data *data = (struct counterexample_search_data *) context;
    unsigned int i;
    double k;

    switch (decision_tree_node_get_type(node)) {
    case DECISION_TREE_LEAF:
    case DECISION_TREE_LEAF_LOG:
        /* Nothing to do */
        break;

    case DECISION_TREE_UNIVARIATE_LINEAR_SPLIT:
        i = decision_tree_univariate_linear_split_get_index(node);
        k = decision_tree_univariate_linear_split_get_threshold(node);

        if (data->x->intervals[i].l <= k) {
            list_append(L, decision_tree_univariate_linear_split_get_left_child(node));
        }
        if (data->x->intervals[i].u > k) {
            list_append(L, decision_tree_univariate_linear_split_get_right_child(node));
        }

        break;
    }
}



/**
 * Rebuilds path from a leaf to its root, refining a #Hyperrectangle.
 *
 * @param[in,out] x #Hyperrectangle to refine
 * @param[in] leaf Leaf
 */
static void leaf_to_hyperrectangle(Hyperrectangle x, const DecisionTreeNode leaf) {
    DecisionTreeNode previous, current = leaf;

    while (!decision_tree_node_is_root(current)) {
        previous = current;
        current = decision_tree_node_get_parent(current);
        const unsigned int i = decision_tree_univariate_linear_split_get_index(current);
        const double k = decision_tree_univariate_linear_split_get_threshold(current);

        if (decision_tree_univariate_linear_split_get_left_child(current) == previous) {
            x->intervals[i].u = min(x->intervals[i].u, k);
        }
        else if (decision_tree_univariate_linear_split_get_right_child(current) == previous) {
            x->intervals[i].l = max(x->intervals[i].u, k);
        }
    }
}



/**
 * Search a counterexample to prove unstability.
 *
 * @param[in,out] status Pointer to stability analysis status
 * @param[in] T #DecisionTree to analyse
 * @param[in] x #Hyperrectangle region to analyse
 */
static void search_counterexample(
    StabilityStatus *status,
    const DecisionTree T,
    const Hyperrectangle x
) {
    DecisionTreeNode leaf = NULL;
    struct counterexample_search_data data;

    data.T = T;
    data.x = x;
    data.concrete_labels = status->labels_a;
    set_create(&data.abstract_labels, set_equality_string);

    depth_first_search(
        (Node *) &leaf,
        decision_tree_get_root(T),
        is_counterexample_leaf,
        compute_reachable_paths,
        &data
    );

    if (leaf != NULL) {
        Hyperrectangle y;
        hyperrectangle_create(&y, hyperrectangle_get_space_size(x));
        hyperrectangle_copy(y, x);
        leaf_to_hyperrectangle(y, leaf);

        status->result = STABILITY_FALSE;
        hyperrectangle_midpoint(status->sample_b, y);

        hyperrectangle_delete(&y);
    }

    set_delete(&data.abstract_labels);
}





/***********************************************************************
 * Public functions.
 **********************************************************************/

void decision_tree_hyperrectangle_is_stable(
    StabilityStatus *status,
    const DecisionTree T,
    const Hyperrectangle x
) {
    const unsigned int has_sample = status->has_sample;

    /* Ensures presence of a sample. */
    if (!has_sample) {
        hyperrectangle_midpoint(status->sample_a, x);
        set_create(&status->labels_a, set_equality_string);
        decision_tree_classify(status->labels_a, T, status->sample_a);
    }
    status->result = STABILITY_DONT_KNOW;


    /* If no counterexamples are found, classifier is robust due to
       completeness */
    search_counterexample(status, T, x);
    if (status->result == STABILITY_DONT_KNOW) {
        status->result = STABILITY_TRUE;
    }


    /* Deallocates memory */
    if (!has_sample) {
        stability_status_unset_sample(status);
    }
}
