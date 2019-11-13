/**
 * Implements a best-first search algorithm.
 *
 * @file best_first.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "best_first.h"

#include "../priority_queue.h"


void best_first_search(
    Node *goal,
    const Node root,
    const NodePredicate is_goal,
    const NodeAdjacencyFunction compute_adjacent_nodes,
    const NodePriorityFunction compute_priority,
    Context context
) {
    PriorityQueue Q;
    List adjacent_nodes;

    priority_queue_create(&Q);
    list_create(&adjacent_nodes);
    priority_queue_push(Q, root, 0.0);

    while (!priority_queue_is_empty(Q)) {
        const Node x = priority_queue_pop(Q);

        if (is_goal(x, context)) {
            *goal = x;
            break;
        }

        compute_adjacent_nodes(adjacent_nodes, x, context);
        while (!list_is_empty(adjacent_nodes)) {
            const Node y = list_pop(adjacent_nodes);
            priority_queue_push(Q, y, compute_priority(y, context));
        }
    }

    priority_queue_delete(&Q);
    list_delete(&adjacent_nodes);
}
