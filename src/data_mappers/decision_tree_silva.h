/**
 * Data mapper for a decision tree to filesystem.
 *
 * @file decision_tree_silva.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef DECISION_TREE_SILVA_H
#define DECISION_TREE_SILVA_H

#include <stdio.h>

#include "../decision_tree.h"


/**
 * Reads a decision tree stored in silva format.
 *
 * @param[out] T Pointer to decision tree
 * @param[in,out] stream Stream
 * @warning #decision_tree_delete should be called to ensure proper
 *          memory deallocation.
 */
void decision_tree_silva_read(DecisionTree *T, FILE *stream);

#endif
