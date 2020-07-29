/**
 * Hyperrectangle analysis for a classifier.
 *
 * @file classifier_hyperrectangle.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef CLASSIFIER_HYPERRECTANGLE_H
#define CLASSIFIER_HYPERRECTANGLE_H

#include "../classifier.h"
#include "../abstract_domains/hyperrectangle.h"
#include "../adversarial_region.h"
#include "stability_status.h"
#include "../tier.h"


/**
 * Asserts whether a classifier is stable in a #Hyperrectangle region.
 *
 * If classifier is provably unstable, a counterexample is given.
 *
 * @param[in,out] result Pointer to #StabilityStatus
 * @param[in] C #Classifier to analyse
 * @param[in] x #Hyperrectangle adversarial region to analyse
 */
void classifier_hyperrectangle_is_stable(
    StabilityStatus *result,
    const Classifier C,
    const AdversarialRegion x,
    const Tier t
);

#endif
