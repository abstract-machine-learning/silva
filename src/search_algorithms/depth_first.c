/**
 * Implements a depth-first search algorithm.
 *
 * @file depth_first.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "depth_first.h"

#include "../stack.h"

void depth_first_search(
    Node *goal,
    const Node root,
    const NodePredicate is_goal,
    const NodeAdjacencyFunction compute_adjacent_nodes,
    Context context
) {
    Stack S;
    List adjacent_nodes;

    (void) goal;

    stack_create(&S);
    list_create(&adjacent_nodes);
    stack_push(S, root);

    while (!stack_is_empty(S)) {
        const Node x = stack_pop(S);

        if (is_goal(x, context)) {
            *goal = x;
            break;
        }

        compute_adjacent_nodes(adjacent_nodes, x, context);
        while (!list_is_empty(adjacent_nodes)) {
            stack_push(S, list_pop(adjacent_nodes));
        }
    }

    stack_delete(&S);
    list_delete(&adjacent_nodes);
}
