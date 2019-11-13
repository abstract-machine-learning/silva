/**
 * Defines a best-first search algorithm.
 *
 * @file best_first.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef BEST_FIRST_H
#define BEST_FIRST_H

#include "search_algorithms.h"

/**
 * Performs a best-first search.
 *
 * @param[out] goal Goal node, if any
 * @param[in] root Starting node
 * @param[in] is_goal Tells whether a node is a goal node
 * @param[in] compute_adjacent_nodes Returns #List of next nodes to visit
 * @param[in] compute_priority Returns estimated priority of a node
 * @param[in,out] context Additional data to be passed to is_goal,
 *                        compute_next_nodes and compute_priority
 */
void best_first_search(
    Node *goal,
    const Node root,
    const NodePredicate is_goal,
    const NodeAdjacencyFunction compute_adjacent_nodes,
    const NodePriorityFunction compute_priority,
    Context context
);

#endif
