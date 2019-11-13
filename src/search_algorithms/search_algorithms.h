/**
 * Defines common data types for search algorithms.
 *
 * @file search_algorithms.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef SEARCH_ALGORITHMS_H
#define SEARCH_ALGORITHMS_H

#include "../list.h"

/** Type of a node. */
typedef void *Node;

/** Type of a context. */
typedef void * const Context;

/** Type of a ndoe predicate. */
typedef unsigned int (*NodePredicate)(const Node, Context);

/** Type of a function returning adjiancent nodes. */
typedef void (*NodeAdjacencyFunction)(List, const Node, Context);

/** Type of a function estimating priority of a node. */
typedef double (*NodePriorityFunction)(const Node, Context);

#endif
