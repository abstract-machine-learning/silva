/**
 * Defines a classifier.
 *
 * @file classifier.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <stdio.h>

#include "decision_tree.h"
#include "forest.h"

/** Types of classifiers. */
typedef enum {
    CLASSIFIER_TREE,   /**< Decision tree classifier. */
    CLASSIFIER_FOREST  /**< Forest classifier. */
} ClassifierType;


/** Type of a classifier. */
typedef struct classifier *Classifier;


/**
 * Creates a decision tree classifier.
 *
 * @param[out] C Pointer to classifier to create
 * @param[in] T Decision tree
 * @warning #classifier_delete should be called to ensure proper memory
 *          deallocation.
 */
void classifier_create_decision_tree(Classifier *C, const DecisionTree T);


/**
 * Creates a forest classifier.
 *
 * @param[out] C Classifier
 * @param[in] F Forest
 * @warning #classifier_delete should be called to ensure proper memory
 *          deallocation.
 */
void classifier_create_forest(Classifier *C, const Forest F);


/**
 * Deletes a classifier.
 *
 * @param[out] C Pointer to classifier to delete
 * @warning Deallocates set of classes and classes.
 */
void classifier_delete(Classifier *C);


/**
 * Returns type of a classifier.
 *
 * @param[in] C Classifier
 * @return Type of classifier
 */
ClassifierType classifier_get_type(const Classifier C);


/**
 * Returns size of the feature space of a classifier.
 *
 * @param[in] C Classifier
 * @return feature space size of the classifier
 */
unsigned int classifier_get_feature_space_size(const Classifier C);

 
/**
 * Returns number of labels of a classifier.
 *
 * @param[in] C Classifier
 * @return Number of labels
 */
unsigned int classifier_get_n_labels(const Classifier C);


/**
 * Returns labels of a classifier.
 * 
 * @param[in] C Classifier
 * @return Array of labels of a classifier
 */
char **classifier_get_labels_as_array(const Classifier C);


/**
 * Computes probabilities of classes of a sample.
 *
 * @param[out] scores Array of scores for given sample
 * @param[in] C Classifier
 * @param[in] x Sample
 */
void classifier_compute_decision_function(
    double *scores,
    const Classifier C,
    const double *x
);


/**
 * Classifies a sample.
 *
 * @param[out] labels Set of labels of the sample
 * @param[in] C Classifier
 * @param[in] x Sample
 */
void classifier_classify(Set labels, const Classifier C, const double *x);


/**
 * Prints a classifier.
 *
 * @param[in] C Classifier
 * @param[out] stream Stream
 */
void classifier_print(const Classifier C, FILE *stream);



/**
 * Returns internal decision tree.
 *
 * @param[in] C Classifier
 * @return Internal decision tree
 */
DecisionTree classifier_get_decision_tree(const Classifier C);


/**
 * Returns internal forest.
 *
 * @param[in] C Classifier
 * @return Internal forest
 */
Forest classifier_get_forest(const Classifier C);

#endif
