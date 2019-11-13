/**
 * Defines a depth-first search algorithm.
 *
 * @file depth_first.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef DEPTH_FIRST_H
#define DEPTH_FIRST_H

#include "search_algorithms.h"

/**
 * Performs a depth-first search.
 *
 * @param[out] goal Goal node, if any
 * @param[in] root Starting node
 * @param[in] is_goal Tells whether a node is a goal node
 * @param[in] compute_adjacent_nodes Returns #List of next nodes to visit
 * @param[in,out] context Additional data to be passed to is_goal and
 *                        compute_adjacent_nodes
 */
void depth_first_search(
    Node *goal,
    const Node root,
    const NodePredicate is_goal,
    const NodeAdjacencyFunction compute_adjacent_nodes,
    Context context
);

#endif
