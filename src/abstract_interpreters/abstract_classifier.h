/**
 * Defines an abstract classifier.
 *
 * @file abstract_classifier.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef ABSTRACT_CLASSIFIER_H
#define ABSTRACT_CLASSIFIER_H

#include <stdio.h>

#include "../classifier.h"
#include "../abstract_domains/abstract_domain.h"
#include "../adversarial_region.h"
#include "stability_status.h"
#include "../tier.h"


/** Type of an abstract classifier. */
typedef struct abstract_classifier *AbstractClassifier;


/**
 * Creates an abstract classifier.
 *
 * @param[out] AC Pointer to abstract classifier to create
 * @param[in] C Concrete classifier
 * @param[in] A Abstract domain
 * @warning #abstract_classifier_delete should be called to ensure
 *          proper memory deallocation.
 */
void abstract_classifier_create(
    AbstractClassifier *AC,
    const Classifier C,
    const AbstractDomain A,
    Tier *t
);


/**
 * Deletes an abstract classifier.
 *
 * @param[out] AC Pointer to abstract classifier to delete
 */
void abstract_classifier_delete(AbstractClassifier *AC);



/**
 * Asserts whether a classifier is stable.
 *
 * If classifier is provably unstable, a counterexample is given.
 *
 * @param[in,out] result Pointer to #StabilityStatus
 * @param[in] AC Abstract classifier to analyse
 * @param[in] x #Hyperrectangle adversarial region to analyse
 */
void abstract_classifier_is_stable(
    StabilityStatus *result,
    const AbstractClassifier AC,
    const AdversarialRegion x
);



/**
 * Prints an abstract classifier.
 *
 * @param[in] AC Abstract classifier
 * @param[out] stream Stream
 */
void abstract_classifier_print(
    const AbstractClassifier AC,
    FILE *stream
);

#endif
