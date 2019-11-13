/**
 * Defines an adversarial region.
 *
 * @file adversarial_region.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef ADVERSARIAL_REGION_H
#define ADVERSARIAL_REGION_H

#include "perturbation.h"

/** Structure of an adversarial region. */
struct adversarial_region {
    const double *sample;             /**< Originator of the adversarial region. */
    const unsigned int space_size;    /**< Size of the space. */
    const Perturbation perturbation;  /**< Perturbation. */
};


/** Type of an adversarial region. */
typedef struct adversarial_region AdversarialRegion;

#endif
