/**
 * Status of a stability analysis.
 *
 * @file stability_status.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef STABILITY_OPTIONS_H
#define STABILITY_OPTIONS_H

#include <stdio.h>
#include "../abstract_domains/hyperrectangle.h"
#include "../set.h"

/** Types of stability analysis status. */
enum stability_result {
    STABILITY_TRUE,      /**< Classifier is stable. */
    STABILITY_FALSE,     /**< Classifier is unstable, counterexample is given. */
    STABILITY_DONT_KNOW  /**< Analysis was inconclusive. */
};


/** Type of a stability analysis result. */
typedef enum stability_result StabilityResult;


/** Structure of a stability analysis status. */
struct stability_status {
    StabilityResult result;   /**< Result of analysis. */
    unsigned int has_sample;  /**< 1 if sample_a is given as input,
                                   0 if sample_a must be comptued. */
    double *sample_a;         /**< First sample. */
    double *sample_b;         /**< Second sample, such that
                                   \f$result = STABILITY\_FALSE \Rightarrow
                                   Cl(sample_A) \neq Cl(sample_B)\f$. */
    Hyperrectangle region;    /**< Hyperrectangle such that \f$\forall x
                                   \in region\colon Cl(x) \neq Cl(sample_A)\f$*/
    Set labels_a;             /**< #Set of labels such that
                                   \f$ has\_sample \Rightarrow
                                   labels_A = Cl(sample_A)\f$. */
    unsigned int timeout;    /**< Maximum execution time for each sample
                                  (seconds). */
};


/** Type of a stability analysis status. */
typedef struct stability_status StabilityStatus;



/**
 * Sets first sample.
 *
 * @param[out] status Pointer to stability status
 * @param[in] sample Sample to set
 * @param[in] labels #Set of labels such that \f$labels = Cl(samples)\f$
 */
static inline void stability_status_set_sample(
    StabilityStatus *status,
    double * const sample,
    Set labels
) {
    status->has_sample = 1;
    status->sample_a = sample;
    status->labels_a = labels;
}



/**
 * Unsets first sample.
 *
 * @param[out] status Pointer to stability status
 */
static inline void stability_status_unset_sample(StabilityStatus *status) {
    status->has_sample = 0;
}


#endif
