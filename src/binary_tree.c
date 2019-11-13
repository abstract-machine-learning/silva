/**
 * Implements a generic binary tree.
 *
 * @file binary_tree.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "binary_tree.h"

#include <stdlib.h>

#include "stack.h"


/***********************************************************************
 * Internal functions and data structures.
 **********************************************************************/

/**
 * Deallocates a node.
 *
 * @param[in,out] N Node
 * @param[in] data Unused
 */
static void deallocator_visitor(BinaryTreeNode N, void * const data) {
    (void) data;

    free(N);
}



/** Structure of a printer visitor data. */
struct printer_visitor_data {
    void *data;                 /**< Additional data to be passed to printer. */
    BinaryTreePrinter printer;  /**< Printer. */
    FILE *stream;               /**< Stream. */
};



/**
 * Printer visitor for a binary tree.
 *
 * @param[in] N Node
 * @param[in,out] visitor_data Additional data
 */
static void printer_visitor(BinaryTreeNode N, void * const visitor_data) {
    const unsigned int depth = binary_tree_node_get_depth(N);
    const struct printer_visitor_data data = *((struct printer_visitor_data * const) visitor_data);
    unsigned int i;

    /* Prints spacing */
    for (i = 0; i < depth; ++i) {
        fprintf(data.stream, "  ");
    }

    fprintf(data.stream, "Node at @%p: ", (void *) N);
    if (data.printer) {
        data.printer(N->data, data.data, data.stream);
    }
    else {
        fprintf(data.stream, "%p", N->data);
    }
    fprintf(data.stream, "\n");
}





/***********************************************************************
 * Public functions.
 **********************************************************************/

void binary_tree_create(BinaryTree *T) {
    BinaryTree t = (BinaryTree) malloc(sizeof(struct binary_tree_node));
    if (t == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    t->data = NULL;
    t->parent = NULL;
    t->left_child = NULL;
    t->right_child = NULL;

    *T = t;
}



void binary_tree_delete(BinaryTree *T) {
    if (T == NULL || *T == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    binary_tree_depth_first_post_visit(*T, deallocator_visitor, NULL);
    *T = NULL;
}



unsigned int binary_tree_node_is_leaf(const BinaryTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return N->left_child == NULL && N->right_child == NULL;
}



unsigned int binary_tree_node_is_root(const BinaryTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return N->parent == NULL;
}



unsigned int binary_tree_node_has_left_child(const BinaryTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return N->left_child != NULL;
}



unsigned int binary_tree_node_has_right_child(const BinaryTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return N->right_child != NULL;
}



unsigned int binary_tree_node_get_depth(const BinaryTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    unsigned int depth = 0;
    BinaryTreeNode parent = N->parent;
    while (parent != NULL) {
        parent = parent->parent;
        ++depth;
    }

    return depth;
}



void *binary_tree_node_get_data(const BinaryTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return N->data;
}



BinaryTreeNode binary_tree_node_get_parent(const BinaryTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return N->parent;
}



BinaryTreeNode binary_tree_node_get_left_child(const BinaryTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return N->left_child;
}



BinaryTreeNode binary_tree_node_get_right_child(const BinaryTreeNode N) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return N->right_child;
}



void binary_tree_node_set_data(BinaryTreeNode N, void * const data) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    N->data = data;
}



void binary_tree_node_set_left_child(BinaryTreeNode N, BinaryTreeNode L) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    N->left_child = L;
    if (L != NULL) {
        L->parent = N;
    }
}



void binary_tree_node_set_right_child(BinaryTreeNode N, BinaryTreeNode R) {
    if (N == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    N->right_child = R;
    if (R != NULL) {
        R->parent = N;
    }
}



void binary_tree_print(
    const BinaryTree T,
    const BinaryTreePrinter printer,
    void * const data,
    FILE *stream
) {
    struct printer_visitor_data visitor_data;

    visitor_data.data = data;
    visitor_data.printer = printer;
    visitor_data.stream = stream;

    binary_tree_depth_first_pre_visit(T, printer_visitor, (void *) &visitor_data);
}





/***********************************************************************
 * Visit algorithms
 **********************************************************************/

void binary_tree_depth_first_pre_visit(
    const BinaryTree T,
    const BinaryTreeVisitor V,
    void * const data
) {
    if (T == NULL || V == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Stack S;

    stack_create(&S);
    stack_push(S, T);
    while (!stack_is_empty(S)) {
        BinaryTreeNode N = stack_pop(S);
        V(N, data);

        if (N->right_child != NULL) {
            stack_push(S, N->right_child);
        }
        if (N->left_child != NULL) {
            stack_push(S, N->left_child);
        }
    }
    stack_delete(&S);
}



void binary_tree_depth_first_in_visit(
    const BinaryTree T,
    const BinaryTreeVisitor V,
    void * const data
) {
    if (T == NULL || V == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Stack S;
    BinaryTreeNode current;

    stack_create(&S);
    stack_push(S, T);
    current = T->left_child;
    while (!stack_is_empty(S) || current != NULL) {
        if (current == NULL) {
            BinaryTreeNode N = stack_pop(S);
            V(N, data);
            current = N->right_child;
        }
        else {
            stack_push(S, current);
            current = current->left_child;
        }
    }
    stack_delete(&S);
}



void binary_tree_depth_first_post_visit(
    const BinaryTree T,
    const BinaryTreeVisitor V,
    void * const data
) {
    if (T == NULL || V == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Stack S, S_prime;

    stack_create(&S);
    stack_create(&S_prime);
    stack_push(S, T);
    while (!stack_is_empty(S)) {
        BinaryTreeNode N = stack_pop(S);
        stack_push(S_prime, N);

        if (N->left_child != NULL) {
            stack_push(S, N->left_child);
        }
        if (N->right_child != NULL) {
            stack_push(S, N->right_child);
        }
    }

    while (!stack_is_empty(S_prime)) {
        V(stack_pop(S_prime), data);
    }
    stack_delete(&S);
    stack_delete(&S_prime);
}
