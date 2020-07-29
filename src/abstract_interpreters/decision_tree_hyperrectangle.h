/**
 * Hyperrectangle analysis for decision trees.
 *
 * @file decision_tree_hyperrectangle.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef DECISION_TREE_HYPERRECTANGLE_H
#define DECISION_TREE_HYPERRECTANGLE_H

#include "../decision_tree.h"
#include "../abstract_domains/hyperrectangle.h"
#include "../tier.h"
#include "stability_status.h"


/**
 * Tells whether a #DecisionTree is stable in a #Hyperrectangle region.
 *
 * If #DecisionTree is provably unstable, a counterexample is given.
 *
 * @param[in,out] status Pointer to stability analysis status
 * @param[in] T #DecisionTree to analyse
 * @param[in] x #Hyperrectangle representing a region
 */
void decision_tree_hyperrectangle_is_stable(
    StabilityStatus *status,
    const DecisionTree T,
    const Hyperrectangle x,
    const Tier t
);

#endif
