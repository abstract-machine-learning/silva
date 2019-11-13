/**
 * Implements a decision tree.
 *
 * A decision tree is a tree where internal nodes dictates with path to
 * follow in order to reach a leaf, which in turns contains the final
 * output.
 *
 * @file decision_tree.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "decision_tree.h"

#include <stdlib.h>
#include <string.h>


/***********************************************************************
 * Function and data structures on decorators for binary tree.
 **********************************************************************/

/**
 * Creates a data payload.
 *
 * @param[out] D Pointer to data payload to create
 * @warning #data_delete should be called to ensure proper memory
 *          deallocation.
 */
static void data_create(Data *D) {
    Data d = (Data) malloc(sizeof(struct node));
    if (d == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    *D = d;
}


/**
 * Deletes data payload.
 *
 * @param[out] D Pointer to data payload to delete
 */
static void data_delete(Data *D) {
    switch ((*D)->type) {
    case DECISION_TREE_LEAF:
        free((*D)->data.leaf.scores);
        break;

    case DECISION_TREE_LEAF_LOG:
        free((*D)->data.leaf_logarithmic.scores);
        break;

    case DECISION_TREE_UNIVARIATE_LINEAR_SPLIT:
        /* Nothing to do */
        break;
    }

    free(*D);
    *D = NULL;
}





/***********************************************************************
 * Internal functions.
 **********************************************************************/

/** Global identifier counter. */
static unsigned int next_available_id = 1;



/**
 * Visitor which deallocates memory.
 *
 * @param[in,out] N Node
 * @param[in] data Unused
 */
static void deallocator_visitor(
    DecisionTreeNode N,
    void * const data
) {
    Data D = binary_tree_node_get_data(N);
    data_delete(&D);

    (void) data;
}



/**
 * Visitor which computes scores.
 *
 * @param[in,out] N Node
 * @param[in,out] scores Array of scores
 */
static void scores_visitor(
    DecisionTreeNode N,
    void * const scores
) {
    unsigned int i;
    Data D = binary_tree_node_get_data(N);

    switch (D->type) {
    case DECISION_TREE_LEAF:
        for (i = 0; i < D->data.leaf.n_labels; ++i) {
            ((unsigned int *) scores)[i] += D->data.leaf.scores[i];
        }
        break;

    case DECISION_TREE_LEAF_LOG:
        for (i = 0; i < D->data.leaf.n_labels; ++i) {
            ((double *) scores)[i] += D->data.leaf_logarithmic.scores[i];
        }
        break;

    case DECISION_TREE_UNIVARIATE_LINEAR_SPLIT:
        /* Do nothing. */
        break;
    }
}



/**
 * Printer for a decision tree.
 *
 * @param[in] D Data
 * @param[in] data Unused
 * @param[out] stream Stream
 */
static void decision_tree_printer(const void * const D, void *data, FILE *stream) {
    unsigned int i, n_samples = 0;
    const Data d = (Data) D;

    (void) data;

    switch (d->type) {
    case DECISION_TREE_LEAF:
        fprintf(stream, "[");
        for (i = 0; i + 1 < d->data.leaf.n_labels; ++i) {
            fprintf(stream, "%u, ", d->data.leaf.scores[i]);
            n_samples += d->data.leaf.scores[i];
        }
        n_samples += d->data.leaf.scores[i];
        fprintf(stream, "%u] (#samples: %u)", d->data.leaf.scores[i], n_samples);
        break;

    case DECISION_TREE_LEAF_LOG:
        fprintf(stream, "[");
        for (i = 0; i + 1 < d->data.leaf_logarithmic.n_labels; ++i) {
            fprintf(stream, "%g, ", d->data.leaf_logarithmic.scores[i]);
        }
        fprintf(stream, "%g] (weight: %g)", d->data.leaf_logarithmic.scores[i], d->data.leaf_logarithmic.weight);
        break;

    case DECISION_TREE_UNIVARIATE_LINEAR_SPLIT:
        fprintf(stream, "x_%u <= %g:", d->data.univariate_linear_split.i, d->data.univariate_linear_split.k);
        break;
    }
}






/***********************************************************************
 * Functions on leaves.
 **********************************************************************/

void decision_tree_leaf_create(
    DecisionTreeNode *leaf,
    unsigned int * const scores,
    const unsigned int n_labels
) {
    Data D;
    unsigned int i;

    binary_tree_create(leaf);
    data_create(&D);
    D->id = next_available_id++;
    D->type = DECISION_TREE_LEAF;
    D->data.leaf.scores = scores;
    D->data.leaf.n_labels = n_labels;
    D->data.leaf.n_samples = scores[0];

    D->data.leaf.max_score = scores[0];
    for (i = 1; i < n_labels; ++i) {
        D->data.leaf.n_samples += scores[i];
        if (scores[i] > D->data.leaf.max_score) {
            D->data.leaf.max_score = scores[i];
        }
    }

    binary_tree_node_set_data(*leaf, D);
}



void decision_tree_leaf_delete(DecisionTreeNode *leaf) {
    if (leaf == NULL || *leaf == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Data D = binary_tree_node_get_data(*leaf);
    data_delete(&D);
    binary_tree_delete(leaf);
}



unsigned int *decision_tree_leaf_get_scores(const DecisionTreeNode leaf) {
    if (leaf == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Data D = binary_tree_node_get_data(leaf);
    if (D->type != DECISION_TREE_LEAF) {
        fprintf(stderr, "[%s: %d] Node is not a leaf.\n", __FILE__, __LINE__);
        abort();
    }

    return D->data.leaf.scores;
}





/***********************************************************************
 * Functions on leaves with logarithmic distribution of probabilities.
 **********************************************************************/

void decision_tree_leaf_logarithmic_create(
    DecisionTreeNode *leaf,
    double * const scores,
    const unsigned int n_labels,
    const double weight
) {
    Data D;

    binary_tree_create(leaf);
    data_create(&D);
    D->id = next_available_id++;
    D->type = DECISION_TREE_LEAF_LOG;
    D->data.leaf_logarithmic.scores = scores;
    D->data.leaf_logarithmic.n_labels = n_labels;
    D->data.leaf_logarithmic.weight = weight;

    binary_tree_node_set_data(*leaf, D);
}



void decision_tree_leaf_logarithmic_delete(DecisionTreeNode *leaf) {
    if (leaf == NULL || *leaf == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Data D = binary_tree_node_get_data(*leaf);
    data_delete(&D);
    binary_tree_delete(leaf);
}



double *decision_tree_leaf_logarithmic_get_scores(const DecisionTreeNode leaf) {
    if (leaf == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Data D = binary_tree_node_get_data(leaf);
    if (D->type != DECISION_TREE_LEAF_LOG) {
        fprintf(stderr, "[%s: %d] Node is not a leaf.\n", __FILE__, __LINE__);
        abort();
    }

    return D->data.leaf_logarithmic.scores;

}





/***********************************************************************
 * Functions on univariate linear split nodes.
 **********************************************************************/

void decision_tree_univariate_linear_split_create(
    DecisionTreeNode *N,
    const unsigned int i,
    const double k
) {
    Data D;

    binary_tree_create(N);
    data_create(&D);
    D->id = next_available_id++;
    D->type = DECISION_TREE_UNIVARIATE_LINEAR_SPLIT;
    D->data.univariate_linear_split.i = i;
    D->data.univariate_linear_split.k = k;
    binary_tree_node_set_data(*N, D);
}



void decision_tree_univariate_linear_split_delete(DecisionTreeNode *N) {
    binary_tree_depth_first_pre_visit(*N, deallocator_visitor, NULL);
}



unsigned int decision_tree_univariate_linear_split_get_index(const DecisionTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Data D = binary_tree_node_get_data(N);
    if (D->type != DECISION_TREE_UNIVARIATE_LINEAR_SPLIT) {
        fprintf(stderr, "[%s: %d] Node is not an univariate linear split.\n", __FILE__, __LINE__);
        abort();
    }
    
    return D->data.univariate_linear_split.i;

}



double decision_tree_univariate_linear_split_get_threshold(const DecisionTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Data D = binary_tree_node_get_data(N);
    if (D->type != DECISION_TREE_UNIVARIATE_LINEAR_SPLIT) {
        fprintf(stderr, "[%s: %d] Node is not an univariate linear split.\n", __FILE__, __LINE__);
        abort();
    }

    return D->data.univariate_linear_split.k;

}



DecisionTreeNode decision_tree_univariate_linear_split_get_left_child(const DecisionTreeNode N) {
    return binary_tree_node_get_left_child(N);
}



DecisionTreeNode decision_tree_univariate_linear_split_get_right_child(const DecisionTreeNode N) {
    return binary_tree_node_get_right_child(N);
}



void decision_tree_univariate_linear_split_set_left_child(
    DecisionTreeNode N,
    const DecisionTreeNode L
) {
    binary_tree_node_set_left_child(N, L);
}



void decision_tree_univariate_linear_split_set_right_child(
    DecisionTreeNode N,
    const DecisionTreeNode R
) {
    binary_tree_node_set_right_child(N, R);
}





/***********************************************************************
 * Functions on generic nodes.
 **********************************************************************/

void decision_tree_node_delete(DecisionTreeNode *N) {
    if (N == NULL || *N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    binary_tree_depth_first_pre_visit(*N, deallocator_visitor, NULL);
    binary_tree_delete(N);
}



unsigned int decision_tree_node_is_leaf(DecisionTreeNode N) {
    return binary_tree_node_is_leaf(N);
}



unsigned int decision_tree_node_is_root(DecisionTreeNode N) {
    return binary_tree_node_is_root(N);
}



unsigned int decision_tree_node_get_identifier(const DecisionTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Data D = binary_tree_node_get_data(N);
    return D->id;
}



DecisionTreeNodeType decision_tree_node_get_type(const DecisionTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Data D = binary_tree_node_get_data(N);
    return D->type;
}



void decision_tree_node_get_scores(unsigned int * const scores, const DecisionTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Data D = binary_tree_node_get_data(N);
    switch (D->type) {
    case DECISION_TREE_LEAF:
        memcpy(scores, D->data.leaf.scores, D->data.leaf.n_labels * sizeof(unsigned int));
        break;
    case DECISION_TREE_LEAF_LOG:
        fprintf(stderr, "[%s, %d] Cannot return scores of a leaf containing a logarithmic distribution.\n", __FILE__, __LINE__);
        abort();
    case DECISION_TREE_UNIVARIATE_LINEAR_SPLIT:
        memset(scores, 0, decision_tree_node_get_n_labels(N) * sizeof(unsigned int));
        binary_tree_depth_first_pre_visit(N, scores_visitor, scores);
        break;
    }
}



void decision_tree_node_get_scores_logarithmic(double * const scores, const DecisionTreeNode N) {
    unsigned int i;

    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Data D = binary_tree_node_get_data(N);
    switch (D->type) {
    case DECISION_TREE_LEAF:
        fprintf(stderr, "[%s, %d] Cannot return scores of a leaf containing number of samples per class.\n", __FILE__, __LINE__);
        abort();
    case DECISION_TREE_LEAF_LOG:
        memcpy(scores, D->data.leaf_logarithmic.scores, D->data.leaf_logarithmic.n_labels * sizeof(double));
        break;
    case DECISION_TREE_UNIVARIATE_LINEAR_SPLIT:
        for (i = 0; i < D->data.leaf_logarithmic.n_labels; ++i) {
            scores[i] = 0.0;
        }
        binary_tree_depth_first_pre_visit(N, scores_visitor, scores);
        break;
    }
}



void decision_tree_node_get_probabilities(double * const P, const DecisionTreeNode N) {
    const unsigned int n_labels = decision_tree_node_get_n_labels(N);
    unsigned int i,
                 n_samples = 0,
                 *scores = (unsigned int *) malloc(n_labels * sizeof(unsigned int));
    if (scores == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    decision_tree_node_get_scores(scores, N);
    for (i = 0; i  < n_labels; ++i) {
        n_samples += scores[i];
    }
    for (i = 0; i < n_labels; ++i) {
        P[i] = (double) scores[i] / (double) n_samples;
    }
    free(scores);
}



unsigned int decision_tree_node_get_n_samples(const DecisionTreeNode N) {
    const unsigned int n_labels = decision_tree_node_get_n_labels(N);
    unsigned int i,
                 n_samples = 0,
                 *scores = (unsigned int *) malloc(n_labels * sizeof(unsigned int));
    if (scores == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    decision_tree_node_get_scores(scores, N);
    for (i = 0; i  < n_labels; ++i) {
        n_samples += scores[i];
    }

    return n_samples;
}



unsigned int decision_tree_node_get_n_labels(const DecisionTreeNode N) {
    DecisionTreeNode current = N;
    Data D;

    while (binary_tree_node_has_left_child(current)) {
        current = binary_tree_node_get_left_child(current);
    }
    D = binary_tree_node_get_data(current);

    return D->data.leaf.n_labels;
}



DecisionTreeNode decision_tree_node_get_parent(const DecisionTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return binary_tree_node_get_parent(N);
}





/***********************************************************************
 * Functions on decision trees.
 **********************************************************************/

void decision_tree_create(
    DecisionTree *T,
    const DecisionTreeNode N,
    const unsigned int n,
    char ** const labels,
    const unsigned int n_labels
) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    DecisionTree t = (DecisionTree) malloc(sizeof(struct decision_tree));
    if (t == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    t->root = N;
    t->space_size = n;
    t->labels = labels;
    t->n_labels = n_labels;

    *T = t;
}



void decision_tree_delete(DecisionTree *T) {
    if (T == NULL || *T == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    unsigned int i;
    decision_tree_node_delete(&(*T)->root);
    for (i = 0; i < (*T)->n_labels; ++i) {
        free((*T)->labels[i]);
    }
    free((*T)->labels);
    free(*T);
    *T = NULL;
}



DecisionTreeNode decision_tree_get_root(const DecisionTree T) {
    if (T == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return T->root;
}



unsigned int decision_tree_get_feature_space_size(const DecisionTree T) {
    if (T == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return T->space_size;
}



char **decision_tree_get_labels_as_array(const DecisionTree T) {
    if (T == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return T->labels;
}



unsigned int decision_tree_get_n_labels(const DecisionTree T) {
    if (T == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return T->n_labels;
}



void decision_tree_compute_decision_function(
    double *scores,
    const DecisionTree T,
    const double *x
) {
    if (scores == NULL || T == NULL || x == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    unsigned int i;
    DecisionTreeNode N = T->root;
    Data D = binary_tree_node_get_data(N);
    while (D->type != DECISION_TREE_LEAF && D->type != DECISION_TREE_LEAF_LOG) {
        switch (D->type) {
        case DECISION_TREE_LEAF:
            /* Impossible */
            break;
        case DECISION_TREE_LEAF_LOG:
            /* Impossible */
            break;
        case DECISION_TREE_UNIVARIATE_LINEAR_SPLIT:
            N = x[D->data.univariate_linear_split.i] <= D->data.univariate_linear_split.k
              ? binary_tree_node_get_left_child(N)
              : binary_tree_node_get_right_child(N);
            break;
        }
        D = binary_tree_node_get_data(N);
    }

    switch (D->type) {
    case DECISION_TREE_LEAF:
        for (i = 0; i < T->n_labels; ++i) {
            scores[i] = (double) D->data.leaf.scores[i] / (double) D->data.leaf.n_samples;
        }
        break;

    case DECISION_TREE_LEAF_LOG:
        for (i = 0; i < T->n_labels; ++i) {
            scores[i] = D->data.leaf_logarithmic.scores[i];
        }
        break;

    case DECISION_TREE_UNIVARIATE_LINEAR_SPLIT:
        /* Impossible */
        break;
    }
}



void decision_tree_classify(
    Set labels,
    const DecisionTree T,
    const double *x
) {
    if (T == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    const unsigned int n_labels = T->n_labels;
    unsigned int i;
    double max, *scores = (double *) malloc(n_labels * sizeof(double));
    if (scores == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    set_clear(labels);
    decision_tree_compute_decision_function(scores, T, x);
    max = scores[0];
    for (i = 1; i < n_labels; ++i) {
        if (scores[i] > max) {
            max = scores[i];
        }
    }

    for (i = 0; i < n_labels; ++i) {
        if (scores[i] == max) {
            set_add_element(labels, T->labels[i]);
        }
    }

    free(scores);
}



void decision_tree_print(const DecisionTree T, FILE *stream) {
    if (T == NULL) {
        fprintf(stream, "NULL decision tree.\n");
        return;
    }

    unsigned int i;
    fprintf(stream, "Decision tree at @%p, feature space size: %u, %u labels: {", (void *) T, T->space_size, T->n_labels);
    for (i = 0; i + 1 < T->n_labels; ++i) {
        fprintf(stream, "%s, ", T->labels[i]);
    }
    fprintf(stream, "%s}.\n", T->labels[i]);

    binary_tree_print(T->root, decision_tree_printer, NULL, stream);
}
