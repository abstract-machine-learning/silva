/**
 * Data mapper for a decision tree to Graphviz.
 *
 * @file decision_tree_graphviz.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "decision_tree_graphviz.h"

#include <stdlib.h>
#include <string.h>


/***********************************************************************
 * Private functions and data structures.
 **********************************************************************/

/**
 * Exports a node in dot format.
 *
 * @param[in] node Node
 * @param[in,out] data Stream
 */
static void graphviz_writer(const BinaryTreeNode node, void *data) {
    FILE *stream = (FILE *) data;

    if (binary_tree_node_is_leaf(node)) {
        const unsigned int n_labels = decision_tree_node_get_n_labels(node),
                           node_id = decision_tree_node_get_identifier(node);
        unsigned int i;

        fprintf(stream, "%u [shape=box label=\"[", node_id);
        for (i = 0; i < n_labels; ++i) {
            fprintf(stream, "%u", decision_tree_leaf_get_scores(node)[i]);
            if (i + 1 < n_labels) {
                fprintf(stream, ", ");
            }
        }
        fprintf(stream, "]\"]\n");
    }

    else {
        const BinaryTreeNode left = decision_tree_univariate_linear_split_get_left_child(node),
                             right = decision_tree_univariate_linear_split_get_right_child(node);
        const unsigned int node_id = decision_tree_node_get_identifier(node),
                           left_id = decision_tree_node_get_identifier(left),
                           right_id = decision_tree_node_get_identifier(right),
                           n_labels = decision_tree_node_get_n_labels(node);
        unsigned int *scores = (unsigned int *) malloc(n_labels * sizeof(unsigned int)),
                     i;
        decision_tree_node_get_scores(scores, node);
        fprintf(
            stream,
            "%u [shape=diamond label=<x<sub>%u</sub> &le; %g <br/>&#91;",
            node_id,
            decision_tree_univariate_linear_split_get_index(node),
            decision_tree_univariate_linear_split_get_threshold(node)
        );
        for (i = 0; i < n_labels; ++i) {
            fprintf(stream, "%u", scores[i]);
            if (i + 1 < n_labels) {
                fprintf(stream, ", ");
            }
        }
        fprintf(stream, "&#93;>]\n");
        fprintf(stream, "%u -> %u [label=\"T\"]\n", node_id, left_id);
        fprintf(stream, "%u -> %u [label=\"F\"]\n", node_id, right_id);

        free(scores);
    }
}


/***********************************************************************
 * Public functions.
 **********************************************************************/

void decision_tree_graphviz_create(
    FILE *stream,
    const DecisionTree tree
) {
    fprintf(stream, "digraph D {\n");
    binary_tree_depth_first_pre_visit(tree->root, graphviz_writer, stream);
    fprintf(stream, "}\n");
}

