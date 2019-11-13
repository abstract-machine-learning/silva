/**
 * Hyperrectangle analysis for forests.
 *
 * @file forest_hyperrectangle.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef FOREST_HYPERRECTANGLE_H
#define FOREST_HYPERRECTANGLE_H

#include "../forest.h"
#include "../abstract_domains/hyperrectangle.h"
#include "stability_status.h"


/**
 * Tells whether a #Forest is stable in a #Hyperrectangle region.
 *
 * If #Forest is proavably unstable, a counterexample is given.
 *
 * @param[in,out] status Pointer to stability analysis status
 * @param[in] F #Forest to analyse
 * @param[in] x #Hyperrectangle representing a region
 */
void forest_hyperrectangle_is_stable(
    StabilityStatus *status,
    const Forest F,
    const Hyperrectangle x
);

#endif
