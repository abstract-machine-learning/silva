/**
 * Implements a classifier.
 *
 * @file classifier.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "classifier.h"

#include <stdlib.h>
#include <string.h>


/** Disjunction of classifier types. */
union internal_data {
    DecisionTree T;  /**< Decision tree model. */
    Forest F;        /**< Forest model. */
};



/** Structure of a classifier. */
struct classifier {
    ClassifierType type;       /**< Type of classifier. */
    union internal_data data;  /**< Internal data. */
};



void classifier_create_decision_tree(Classifier *C, const DecisionTree T) {
    Classifier c = (Classifier) malloc(sizeof(struct classifier));
    if (c == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    c->type = CLASSIFIER_TREE;
    c->data.T = T;

    *C = c;
}



void classifier_create_forest(Classifier *C, const Forest F) {
    Classifier c = (Classifier) malloc(sizeof(struct classifier));
    if (c == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    c->type = CLASSIFIER_FOREST;
    c->data.F = F;

    *C = c;
}



void classifier_delete(Classifier *C) {
    if (C == NULL || *C == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    switch ((*C)->type) {
        case CLASSIFIER_TREE:
            decision_tree_delete(&(*C)->data.T);
            break;

        case CLASSIFIER_FOREST:
            forest_delete(&(*C)->data.F);
            break;
    }

    free(*C);
    *C = NULL;
}



ClassifierType classifier_get_type(const Classifier C) {
    if (C == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return C->type;
}



unsigned int classifier_get_feature_space_size(const Classifier C) {
    if (C == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    switch (C->type) {
    case CLASSIFIER_TREE:
        return decision_tree_get_feature_space_size(C->data.T);
    case CLASSIFIER_FOREST:
        return forest_get_feature_space_size(C->data.F);
    }

    fprintf(stderr, "[%s: %d] Unsupported type of classifier.\n", __FILE__, __LINE__);
    abort();
}



unsigned int classifier_get_n_labels(const Classifier C) {
    if (C == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    switch (C->type) {
    case CLASSIFIER_TREE:
        return decision_tree_get_n_labels(C->data.T);
    case CLASSIFIER_FOREST:
        return forest_get_n_labels(C->data.F);
    }

    fprintf(stderr, "[%s: %d] Unsupported type of classifier.\n", __FILE__, __LINE__);
    abort();
}



char **classifier_get_labels_as_array(const Classifier C) {
    if (C == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    switch (C->type) {
    case CLASSIFIER_TREE:
        return decision_tree_get_labels_as_array(C->data.T);
    case CLASSIFIER_FOREST:
        return forest_get_labels_as_array(C->data.F);
    }

    fprintf(stderr, "[%s: %d] Unsupported type of classifier.\n", __FILE__, __LINE__);
    abort();
}



void classifier_compute_decision_function(
    double *scores,
    const Classifier C,
    const double *x
) {
    if (C == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    switch (C->type) {
    case CLASSIFIER_TREE:
        decision_tree_compute_decision_function(scores, C->data.T, x);
        break;
    case CLASSIFIER_FOREST:
        forest_compute_decision_function(scores, C->data.F, x);
        break;
    }
}



void classifier_classify(Set labels, const Classifier C, const double *x) {
    if (C == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    switch (C->type) {
    case CLASSIFIER_TREE:
        decision_tree_classify(labels, C->data.T, x);
        break;
    case CLASSIFIER_FOREST:
        forest_classify(labels, C->data.F, x);
        break;
    }
}



void classifier_print(const Classifier C, FILE *stream) {
    if (C == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    fprintf(stream, "Calssifier @%p:\n", (void *) C);
    switch (C->type) {
    case CLASSIFIER_TREE:
        decision_tree_print(C->data.T, stream);
        break;
    case CLASSIFIER_FOREST:
        forest_print(C->data.F, stream);
        break;
    }
}



DecisionTree classifier_get_decision_tree(const Classifier C) {
    if (C == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    if (C->type != CLASSIFIER_TREE) {
        fprintf(stderr, "[%s: %d] Classifier is not a decision tree.\n", __FILE__, __LINE__);
        abort();
    }

    return C->data.T;
}



Forest classifier_get_forest(const Classifier C) {
    if (C == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    if (C->type != CLASSIFIER_FOREST) {
        fprintf(stderr, "[%s: %d] Classifier is not a forest.\n", __FILE__, __LINE__);
        abort();
    }

    return C->data.F;
}

