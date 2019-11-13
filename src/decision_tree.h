/**
 * Defines a decision tree.
 *
 * A decision tree is a tree where internal nodes dictates with path to
 * follow in order to reach a leaf, which in turns contains the final
 * output.
 *
 * @file decision_tree.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include <stdio.h>

#include "set.h"
#include "binary_tree.h"


/** Type of a decision tree. */
typedef struct decision_tree *DecisionTree;


/** Type of a decision tree node. */
typedef BinaryTreeNode DecisionTreeNode;


/** Types of node. */
typedef enum {
    DECISION_TREE_LEAF,                    /**< Leaf node. */
    DECISION_TREE_LEAF_LOG,                /**< Leaf with logarithmic
                                                distribution of probabilities. */
    DECISION_TREE_UNIVARIATE_LINEAR_SPLIT  /**< Split node in the form
                                                \f$x_i \leq k \f$. */
} DecisionTreeNodeType;



/** Structure of a decision tree. */
struct decision_tree {
    DecisionTreeNode root;    /**< Root of the binary tree. */
    unsigned int space_size;  /**< Size of the feature space. */
    char **labels;            /**< Array of labels. */
    unsigned int n_labels;    /**< Number of labels. */
};



/***********************************************************************
 * Function and data structures on decorators for binary tree.
 **********************************************************************/

/** Type of a data payload. */
typedef struct node *Data;

/** Structure of a leaf. */
struct leaf { 
    unsigned int *scores;    /**< Array of scores. */
    unsigned int n_labels;   /**< Number of labels. */
    unsigned int max_score;  /**< Maximum score. */
    unsigned int n_samples;  /**< Total number of samples in the leaf. */
}; 

/** Structure of a leaf containing logarithmic distribution of probabilities. */
struct leaf_logarithmic {
    double *scores;         /**< Array of scores. */
    unsigned int n_labels;  /**< Number of labels. */
    double weight;          /**< Weight of the leaf. */
};


/** Structure of an univariate linear split. */
struct univariate_linear_split {
    unsigned int i;  /**< Index of dimension. */
    double k;        /**< Threshold. */
};
    

/** Disjunction of types of node. */ 
union node_disjunction {
    struct leaf leaf;  /**< Leaf. */
    struct leaf_logarithmic leaf_logarithmic;  /**< Leaf with logarithmic probability distribution. */
    struct univariate_linear_split  univariate_linear_split;  /**< Univariate linear split. */
};  


/** Structure of a decision tree node data. */
struct node {
    unsigned int id;              /**< Identifier of node. */
    DecisionTreeNodeType type;    /**< Type of node. */
    union node_disjunction data;  /**< Type-dependent data. */
};




/***********************************************************************
 * Functions on leaves.
 **********************************************************************/

/**
 * Creates a leaf.
 *
 * @param[out] leaf Pointer to leaf to create.
 * @param[in] scores Array of scores
 * @param[in] n_labels Number of labels
 * @warning #decision_tree_leaf_delete should be called to ensure proper
 *          memory deallocation.
 */
void decision_tree_leaf_create(
    DecisionTreeNode *leaf,
    unsigned int * const scores,
    const unsigned int n_labels
);


/**
 * Deletes a leaf.
 *
 * @param[out] leaf Pointer to leaf to delete
 */
void decision_tree_leaf_delete(DecisionTreeNode *leaf);



/**
 * Returns array of scores.
 *
 * @param[in] leaf Leaf
 * @return scores Array of score
 */
unsigned int *decision_tree_leaf_get_scores(const DecisionTreeNode leaf);





/***********************************************************************
 * Function on leaves with logarithmic distribution of probabilities.
 **********************************************************************/

/**
 * Creates a leaf.
 *
 * @param[out] leaf Pointer to leaf to create.
 * @param[in] scores Array of logarithmic distribution of probabilities
 * @param[in] n_labels Number of labels
 * @param[in] weight Weight of the leaf
 * @warning #decision_tree_leaf_logarithmic_delete should be called to
 *          ensure proper memory deallocation.
 */
void decision_tree_leaf_logarithmic_create(
    DecisionTreeNode *leaf,
    double * const scores,
    const unsigned int n_labels,
    const double weight
);



/**
 * Deletes a leaf.
 *
 * @param[out] leaf Pointer to leaf to delete
 */
void decision_tree_leaf_logarithmic_delete(DecisionTreeNode *leaf);



/**
 * Returns array of scores.
 *
 * @param[in] leaf Leaf
 * @return scores Array of score
 */
double *decision_tree_leaf_logarithmic_get_scores(const DecisionTreeNode leaf);





/***********************************************************************
 * Functions on univariate linear split nodes.
 **********************************************************************/

/**
 * Creates an univariate linear split node in the form \f$x_i \leq k\f$.
 *
 * @param[out] N Pointer to univariate linear split node to create
 * @param[in] i Index of dimension
 * @param[in] k Threshold
 * @warning #decision_tree_univariate_linear_split_delete should be called
 *          to ensure proper memory deallocation.
 */
void decision_tree_univariate_linear_split_create(
    DecisionTreeNode *N,
    const unsigned int i,
    const double k
);


/**
 * Deletes an univariate linear split node.
 *
 * @param[out] N Pointer to univariate linear split node to delete
 * @note Children of given node are recursively deleted as well
 */
void decision_tree_univariate_linear_split_delete(DecisionTreeNode *N);



/**
 * Returns index of an univariate linear split.
 *
 * @param[in] N Univariate linear split node
 * @return Index of univariate linear split
 */
unsigned int decision_tree_univariate_linear_split_get_index(const DecisionTreeNode N);


/**
 * Returns threshold of an univariate linear split.
 *
 * @param[in] N Univariate linear split node
 * @return Threshold of univariate linear split
 */
double decision_tree_univariate_linear_split_get_threshold(const DecisionTreeNode N);


/**
 * Returns left child of an univariate linear split node.
 *
 * @param[in] N Univariate linear split node
 * @return Left child of univariate linear split node
 */
DecisionTreeNode decision_tree_univariate_linear_split_get_left_child(const DecisionTreeNode N);


/**
 * Returns right child of an univariate linear split node.
 *
 * @param[in] N Univariate linear split node
 * @return Right child of univariate linear split node
 */
DecisionTreeNode decision_tree_univariate_linear_split_get_right_child(const DecisionTreeNode N);



/**
 * Sets left child of an univariate linear split node.
 *
 * @param[out] N Univariate linear split node
 * @param[in] L Left child of univariate linear split node
 */
void decision_tree_univariate_linear_split_set_left_child(
    DecisionTreeNode N,
    const DecisionTreeNode L
);



/**
 * Sets right child of an univariate linear split node.
 *
 * @param[out] N Univariate linear split node
 * @param[in] R Right child of univariate linear split node
 */
void decision_tree_univariate_linear_split_set_right_child(
    DecisionTreeNode N,
    const DecisionTreeNode R
);





/***********************************************************************
 * Functions on generic nodes.
 **********************************************************************/

/**
 * Deletes a node.
 *
 * @param[out] N Pointer to node to delete
 * @note Children of given node, if any, are recursively deleted as well
 */
void decision_tree_node_delete(DecisionTreeNode *N);



/**
 * Tells whether a node is a leaf.
 *
 * @param[in] N Node
 * @return 1 if node is a leaf, 0 othwerise
 */
unsigned int decision_tree_node_is_leaf(DecisionTreeNode N);


/**
 * Tells whether a node is a root.
 *
 * @param[in] N Node
 * @return 1 if node is a root, 0 otherwise
 */
unsigned int decision_tree_node_is_root(DecisionTreeNode N);



/**
 * Returns identifier of a node.
 *
 * @param[in] N Node
 * @return Identifier of node
 */
unsigned int decision_tree_node_get_identifier(const DecisionTreeNode N);


/**
 * Returns type of a node.
 *
 * @param[in] N Node
 * @return Type of node
 */
DecisionTreeNodeType decision_tree_node_get_type(const DecisionTreeNode N);


/**
 * Returns scores of a node.
 *
 * @param[out] scores Array of scores
 * @param[in] N Node
 */
void decision_tree_node_get_scores(unsigned int * const scores, const DecisionTreeNode N);


/**
 * Returns logarithmic scores of a node.
 *
 * @param[out] scores Array of scores
 * @param[in] N Node
 */
void decision_tree_node_get_scores_logarithmic(double * const scores, const DecisionTreeNode N);


/**
 * Returns scores in a node as probabilities.
 *
 * @param[out] P Array of probabilities
 * @param[in] N Node
 */
void decision_tree_node_get_probabilities(double * const P, const DecisionTreeNode N);


/**
 * Returns number of samples in a node.
 *
 * @param[in] N Node
 * @return Total number of samples in a node
 */
unsigned int decision_tree_node_get_n_samples(const DecisionTreeNode N);


/**
 * Returns number of labels in a node.
 *
 * @param[in] N Node
 * @return Number of labels
 */
unsigned int decision_tree_node_get_n_labels(const DecisionTreeNode N);


/**
 * Returns parent of given node.
 *
 * @param[in] N Node
 * @return Parent of node
 */
DecisionTreeNode decision_tree_node_get_parent(const DecisionTreeNode N);





/***********************************************************************
 * Functions on decision trees.
 **********************************************************************/

/**
 * Creates a decision tree.
 *
 * @param[out] T Pointer to decision tree to create
 * @param[in] N Pointer to the root decision tree node
 * @param[in] n Size of feature space
 * @param[in] labels Array of labels
 * @param[in] n_labels Number of labels
 * @warning #decision_tree_delete should be called to ensure proper memory
 *          deallocation.
 */
void decision_tree_create(
    DecisionTree *T,
    const DecisionTreeNode N,
    const unsigned int n,
    char ** const labels,
    const unsigned int n_labels
);


/**
 * Deletes a decision tree.
 *
 * @param[out] T Pointer to tree to delete
 */
void decision_tree_delete(DecisionTree *T);



/**
 * Returns root node of decision tree.
 *
 * @param[in] T Decision tree
 * @return Root of decsion tree
 */
DecisionTreeNode decision_tree_get_root(const DecisionTree T);


/**
 * Returns size of feature space of a decision tree.
 *
 * @param[in] T Decision tree
 * @return Size of feature space
 */
unsigned int decision_tree_get_feature_space_size(const DecisionTree T);


/**
 * Returns labels in a decision tree.
 *
 * @param[in] T Decision tree
 * @return Labels in the decision tree
 */
char **decision_tree_get_labels_as_array(const DecisionTree T);


/**
 * Returns number of labels in a decision tree.
 *
 * @param[in] T Decision tree
 * @return Number of labels
 */
unsigned int decision_tree_get_n_labels(const DecisionTree T);



/**
 * Computes decision function on a sample.
 *
 * @param[out] scores Array of scores for given sample
 * @param[in] T Decision tree
 * @param[in] x Sample
 */
void decision_tree_compute_decision_function(
    double *scores,
    const DecisionTree T,
    const double *x
);


/**
 * Classifies a sample.
 *
 * @param[out] labels #Set of labels
 * @param[in] T Decision tree
 * @param[in] x Sample
 */
void decision_tree_classify(
    Set labels,
    const DecisionTree T,
    const double *x
);



/**
 * Prints a decision tree.
 *
 * @param[in] T Decision tree to print
 * @param[out] stream
 */
void decision_tree_print(const DecisionTree T, FILE *stream);

#endif
