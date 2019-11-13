/**
 * Defines a generic binary tree.
 *
 * @file binary_tree.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <stdio.h>

/** Type of a binary tree. */
typedef struct binary_tree_node *BinaryTree;

/** Type of a node. */
typedef BinaryTree BinaryTreeNode;

/** Type of a leaf. */
typedef BinaryTree BinaryTreeLeaf;


/** Structure of a binary tree node. */
struct binary_tree_node {
    void *data;                  /**< Data stored in the node. */
    BinaryTreeNode parent;       /**< Parent of the node. */
    BinaryTreeNode left_child;   /**< Left child of the node. */
    BinaryTreeNode right_child;  /**< Right child of the node. */
};


/** Type of a binary tree printer. */
typedef void (*BinaryTreePrinter)(const void * const, void * const, FILE *);


/** Type of a tree visitor. */
typedef void (*BinaryTreeVisitor)(BinaryTreeNode, void * const);


/**
 * Creates a binary tree.
 *
 * @param[out] T Pointer to binary tree to create
 * @warning #binary_tree_delete must be called to ensure proper memory
 *          deallocation.
 */
void binary_tree_create(BinaryTree *T);


/**
 * Deletes a binary tree.
 *
 * @param[out] T Pointer to binary tree to delete
 * @warning Every node in the tree is deallocated.
 */
void binary_tree_delete(BinaryTree *T);



/**
 * Tells whether a node is a leaf.
 *
 * @param[in] N Node
 * @return 1 if node is a leaf, 0 otherwise
 */
unsigned int binary_tree_node_is_leaf(const BinaryTreeNode N);


/**
 * Tells whether a node is a root.
 *
 * @param[in] N Node
 * @return 1 if node is a root, 0 otherwise
 */
unsigned int binary_tree_node_is_root(const BinaryTreeNode N);


/**
 * Tells whether a node has a left child.
 *
 * @param[in] N Node
 * @return 1 if node has a left child, 0 otherwise
 */
unsigned int binary_tree_node_has_left_child(const BinaryTreeNode N);


/**
 * Tells whether a node has a right child.
 *
 * @param[in] N Node
 * @return 1 if node has a right child, 0 otherwise
 */
unsigned int binary_tree_node_has_right_child(const BinaryTreeNode N);



/**
 * Returns depth of a node.
 *
 * @param[in] N Node
 * @return Number of edges from root to node
 */
unsigned int binary_tree_node_get_depth(const BinaryTreeNode N);


/**
 * Returns data stored in a node.
 *
 * @param[in] N Node
 * @return Data stored in the node
 */
void *binary_tree_node_get_data(const BinaryTreeNode N);


/**
 * Returns parent of a node.
 *
 * @param[in] N Node
 * @return Parent of the node
 * @note Returns NULL if node is a root.
 */
BinaryTreeNode binary_tree_node_get_parent(const BinaryTreeNode N);


/**
 * Returns left child of a node.
 *
 * @param[in] N Node
 * @return Left child of the node
 * @note Returns NULL if node does not have a left child.
 */
BinaryTreeNode binary_tree_node_get_left_child(const BinaryTreeNode N);


/**
 * Returns right child of a node.
 *
 * @param[in] N Node
 * @return Right child of the node
 * @note Returns NULL if node does not have a right child.
 */
BinaryTreeNode binary_tree_node_get_right_child(const BinaryTreeNode N);



/**
 * Stores data into a node.
 *
 * @param[out] N Node
 * @param[in] data Data
 */
void binary_tree_node_set_data(BinaryTreeNode N, void * const data);


/**
 * Sets left child of a node.
 *
 * @param[out] N Node
 * @param[in,out] L Left child
 */
void binary_tree_node_set_left_child(BinaryTreeNode N, BinaryTreeNode L);


/**
 * Sets right child of a node.
 *
 * @param[out] N Node
 * @param[in,out] R Right child
 */
void binary_tree_node_set_right_child(BinaryTreeNode N, BinaryTreeNode R);



/**
 * Prints a binary tree.
 *
 * @param[in] T Binary tree
 * @param[in] printer Printer
 * @param[in,out] data Additional data to pass to printer
 * @param[out] stream Stream
 */
void binary_tree_print(
    const BinaryTree T,
    const BinaryTreePrinter printer,
    void * const data,
    FILE *stream
);



/***********************************************************************
 * Visit algorithms
 **********************************************************************/

/**
 * Performs a depth-first pre-order visit.
 *
 * @param[in] T Binary tree
 * @param[in] V Visitor
 * @param[in,out] data Additional data to be passed to visitor
 */
void binary_tree_depth_first_pre_visit(
    const BinaryTree T,
    const BinaryTreeVisitor V,
    void * const data
);


/**
 * Performs a depth-first in-order visit.
 *
 * @param[in] T Binary tree
 * @param[in] V Visitor
 * @param[in,out] data Additional data to be passed to visitor
 */
void binary_tree_depth_first_in_visit(
    const BinaryTree T,
    const BinaryTreeVisitor V,
    void * const data
);


/**
 * Performs a depth-first post-order visit.
 *
 * @param[in] T Binary tree
 * @param[in] V Visitor
 * @param[in,out] data Additional data to be passed to visitor
 */
void binary_tree_depth_first_post_visit(
    const BinaryTree T,
    const BinaryTreeVisitor V,
    void * const data
);

#endif
