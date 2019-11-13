/**
 * Data mapper for a decision tree to Graphviz.
 *
 * @file decision_tree_graphviz.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef DECISION_TREE_GRAPHVIZ_H
#define DECISION_TREE_GRAPHVIZ_H

#include <stdio.h>

#include "../decision_tree.h"


/**
 * Creates a decision tree in Graphviz format.
 *
 * @param[out] stream Stream
 * @param[in] tree Decision tree
 */
void decision_tree_graphviz_create(
    FILE *stream,
    const DecisionTree tree
);

#endif
