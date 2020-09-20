/**
 * Hyperrectangle analysis for forests.
 *
 * @file forest_hyperrectangle.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "forest_hyperrectangle.h"

#include <stdlib.h>
#include <float.h>
#include <time.h>

#include "../list.h"
#include "../stack.h"
#include "../priority_queue.h"
#include "../search_algorithms/best_first.h"


/** Machine precision. */
#define EPSILON 1e-12


/** Container size. */
#define CONTAINER_SIZE (1024 * 1024)


/***********************************************************************
 * Data structures shared among the analysis.
 **********************************************************************/

/** Internal status. */
enum internal_status {
    DONT_KNOW,  /**< Analysis has not discovered any information. */
    UNSTABLE,   /**< Unstability was proved. */
    ABORTED     /**< Analysis was aborted (for example due to timeout). */
};

/** Type of internal status. */
typedef enum internal_status InternalStatus;


/** Common data used during analysis. */
struct analysis_data {
    StabilityStatus *status;         /**< Pointer to stability status. */
    Forest F;                        /**< #Forest. */
    time_t start_time;               /**< Start time of analysis. */
    unsigned int timeout;            /**< Maximum execution time per sample. */
    InternalStatus internal_status;  /**< Current status. */
    char * const *labels;            /**< Set of labels, as array. */
    unsigned int n_labels;           /**< Number of labels. */
    unsigned int n_trees;            /**< Number of trees. */
    DecisionTreeNode *S;             /**< Stack of nodes. */
    DecisionTreeNode *L;             /**< List of nodes. */
    unsigned int *local_scores;      /**< Array of integer scores. */
    Set local_labels;                /**< Set of labels for local use. */
    Tier tier;                       /**< Feature tiers. */
};


/** Type of analsis data. */
typedef struct analysis_data * const AnalysisData;





/***********************************************************************
 * Support functions related to decision trees.
 **********************************************************************/

/**
 * Computes set of reachable leaves in a decision tree.
 *
 * Reachable leaves are computed using a depth-first visit.
 *
 * @param[out] L List of reachable leaves, as array
 * @param[out] n_leaves Number of reachable leaves
 * @param[in] T #DecisionTree to explore
 * @param[in] x #Hyperrectangle region to analyse
 * @param[in,out] S Stack to use during depth-first visit
 * @note Stack S is given as input in order to recycle previously allocated
 *       memory.
 */
static void reachable_leaves(
    DecisionTreeNode * const L,
    unsigned int * const n_leaves,
    DecisionTreeNode * const S,
    const DecisionTree const T,
    const Hyperrectangle const x
) {
    DecisionTreeNode n = T->root;
    unsigned int size = 0, list_size = 0;
    const Interval * const intervals = x->intervals;

    S[size] = n;
    ++size;
    while (size) {
        unsigned int i;
        double k;
        n = S[size - 1];
        const struct node data = *((struct node *) n->data);
        --size;

        switch (data.type) {
        /* Leaf: adds it to list */
        case DECISION_TREE_LEAF:
        case DECISION_TREE_LEAF_LOG:
            L[list_size] = n;
            ++list_size;
            break;

        /* Univariate linear split: visits left, right or both */
        case DECISION_TREE_UNIVARIATE_LINEAR_SPLIT:
            i = data.data.univariate_linear_split.i;
            k = data.data.univariate_linear_split.k;

            if (intervals[i].l <= k) {
                S[size] = n->left_child;
                ++size;
            }
            if (intervals[i].u > k) {
                S[size] = n->right_child;
                ++size;
            }
            break;
        }
    }

    *n_leaves = list_size;
}



/**
 * Converts scores overapproximation to a #Set of labels.
 *
 * @param[out] labels #Set of labels
 * @param[in] scores Overapproximation of scores as #Hyperrectangle
 * @param[in] data Analysis data
 */
static void scores_to_labels(
    Set labels,
    const Hyperrectangle scores,
    const AnalysisData data
) {
    const unsigned int n_labels = data->n_labels;
    char * const * const labels_array = data->labels;
    unsigned int i, j;

    set_clear(labels);
    for (i = 0; i < n_labels; ++i) {
        unsigned int is_maximal = 1;

        if (set_has_element(labels, labels_array[i])) {
            continue;
        }

        for (j = 0; j < n_labels; ++j) {
            if (i == j) {
                continue;
            }

            if (interval_is_lt(scores->intervals[i], scores->intervals[j])) {
                is_maximal = 0;
                break;
            }
        }

        if (is_maximal) {
            set_add_element(labels, labels_array[i]);
        }
    }
}





/***********************************************************************
 * Functions and data structures related to hyperrectangle decorators.
 **********************************************************************/

/** Type of a hyperrectangle decorator. */
typedef struct hyperrectangle_decorator *HyperrectangleDecorator;

/** Structure of a hyperrectangle decorator. */
struct hyperrectangle_decorator {
    Hyperrectangle x;                /**< Constraints, as #Hyperrectangle. */
    DecisionTreeNode leaf;           /**< Leaf information. */
    HyperrectangleDecorator parent;  /**< Parent decorator. */
    List children;                   /**< #List of children decorators. */
    Set labels;                      /**< Overapproximation of #Set of labels
                                          of points in #Hyperrectangle. */
};



/**
 * Creates a hyperrectangle decorator.
 *
 * @param[out] x Pointer to hyperrectangle decorator to create
 * @param[in] h #Hyperrectangle
 * @param[in] leaf Leaf of a decision tree
 * @param[in] parent Parent decorator
 * @warning #decorator_delete should be called to ensure proper memory
 *          deallocation.
 */
static void decorator_create(
    HyperrectangleDecorator *x,
    const Hyperrectangle h,
    const DecisionTreeNode leaf,
    const HyperrectangleDecorator parent
) {
    HyperrectangleDecorator d = (HyperrectangleDecorator) malloc(sizeof(struct hyperrectangle_decorator));
    d->x = h;
    d->leaf = leaf;
    d->parent = parent;
    list_create(&d->children);
    set_create(&d->labels, set_equality_string);

    *x = d;
}



/**
 * Deletes a hyperrectangle decorator.
 *
 * @param[out] x Pointer to decorator to delete
 * @warning Children of the decorator are recursively deleted
 */
static void decorator_delete(HyperrectangleDecorator *x) {
    Stack S;

    stack_create(&S);
    stack_push(S, *x);
    while (!stack_is_empty(S)) {
        HyperrectangleDecorator x = stack_pop(S);
        if (x->x)
        hyperrectangle_delete(&x->x);
        while (!list_is_empty(x->children)) {
            stack_push(S, list_pop(x->children));
        }
        list_delete(&x->children);
        set_delete(&x->labels);
        free(x);
    }
    stack_delete(&S);
    *x = NULL;
}



/**
 * Returns depth of a hyperrectangle decorator.
 *
 * @param[in] x Decorator
 * @return Depth of decorator
 */
static unsigned int decorator_get_depth(const HyperrectangleDecorator x) {
    unsigned int depth = 0;
    HyperrectangleDecorator parent = x->parent;

    while (parent) {
        parent = parent->parent;
        ++depth;
    }

    return depth;
}



/**
 * Computes score of a decorator using the max voting sscheme.
 * 
 * Only leaves which are guaranteed to be reached are considered.
 *
 * @param[out] scores #Hyperrectangle of scores
 * @param[in] x Decorator to analyse
 * @param[in] data Analysis data
 */
static void decorator_score_concrete_max(
    Hyperrectangle scores,
    const HyperrectangleDecorator x,
    const AnalysisData data
) {
    const unsigned int n_labels = data->n_labels;
    unsigned int i;
    HyperrectangleDecorator current = x;
    (void) data;

    /* Initializes scores */
    for (i = 0; i < n_labels; ++i) {
        scores->intervals[i].l = 0.0;
        scores->intervals[i].u = 0.0;
    }

    /* Updates scores with information from each leaf */
    while (current->leaf) {
        const unsigned int *leaf_scores = ((struct node *) current->leaf->data)->data.leaf.scores;
        unsigned int max = ((struct node *) current->leaf->data)->data.leaf.max_score;
        for (i = 0; i < n_labels; ++i) {
            if (leaf_scores[i] == max) {
                scores->intervals[i].l += 1.0;
                scores->intervals[i].u += 1.0;
            }
        }

        current = current->parent;
    }
}



/**
 * Computes score of a decorator using the average voting scheme.
 * 
 * Only leaves which are guaranteed to be reached are considered.
 *
 * @param[out] scores #Hyperrectangle of scores
 * @param[in] x Decorator to analyse
 * @param[in] data Analysis data
 */
static void decorator_score_concrete_average(
    Hyperrectangle scores,
    const HyperrectangleDecorator x,
    const AnalysisData data
) {
    const unsigned int n_labels = forest_get_n_labels(data->F);
    unsigned int i;
    HyperrectangleDecorator current = x;

    /* Initializes scores */
    for (i = 0; i < n_labels; ++i) {
        scores->intervals[i].l = 0.0;
        scores->intervals[i].u = 0.0;
    }

    /* Updates scores with information from each leaf */
    while (current->leaf) {
        const unsigned int *leaf_scores = ((struct node *) current->leaf->data)->data.leaf.scores;
        const unsigned int n_samples = ((struct node *) current->leaf->data)->data.leaf.n_samples;

        for (i = 0; i < n_labels; ++i) {
            scores->intervals[i].l += (double) leaf_scores[i] / (double) n_samples / (double) data->n_trees;
            scores->intervals[i].u += (double) leaf_scores[i] / (double) n_samples / (double) data->n_trees;
        }

        current = current->parent;
    }
}



/**
 * Computes score of a decorator using the softargmax voting scheme.
 *
 * Only leaves which are guaranteed to be reached are considered.
 *
 * @param[out] scores #Hyperrectangle of scores
 * @param[in] x Decorator to analyse
 * @param[in] data Analysis data
 */
static void decorator_score_concrete_softargmax(
    Hyperrectangle scores,
    const HyperrectangleDecorator x,
    const AnalysisData data
) {
    const unsigned int n_labels = forest_get_n_labels(data->F);
    unsigned int i;
    HyperrectangleDecorator current = x;

    /* Initializes scores */
    for (i = 0; i < n_labels; ++i) {
        scores->intervals[i].l = 0.0;
        scores->intervals[i].u = 0.0;
    }

    /* Updates scores with information from each leaf */
    while (current->leaf) {
        const double *leaf_scores = ((struct node *) current->leaf->data)->data.leaf_logarithmic.scores;

        for (i = 0; i < n_labels; ++i) {
            scores->intervals[i].l += leaf_scores[i];
            scores->intervals[i].u += leaf_scores[i];
        }

        current = current->parent;
    }
}



/**
 * Computes score of a decorator.
 * 
 * Only leaves which are guaranteed to be reached are considered. Voting
 * scheme depends on the forest. 
 *
 * @param[out] scores #Hyperrectangle of scores
 * @param[in] x Decorator to analyse
 * @param[in] data Analysis data
 */
static void decorator_score_concrete(
    Hyperrectangle scores,
    const HyperrectangleDecorator x,
    const AnalysisData data
) {
    switch (forest_get_voting_scheme(data->F)) {
    case FOREST_VOTING_MAX:
        decorator_score_concrete_max(scores, x, data);
        break;

    case FOREST_VOTING_AVERAGE:
        decorator_score_concrete_average(scores, x, data);
        break;

    case FOREST_VOTING_SOFTARGMAX:
        decorator_score_concrete_softargmax(scores, x, data);
        break;
    }
}



/**
 * Updates score of a decorator using the max voting scheme.
 *
 * Computes set of reachable leaves in a single tree and uses abstract
 * interpretation to overapproximate scores.
 *
 * @param[out] scores #Hyperrectangle of scores
 * @param[in] x Decorator to analyse
 * @param[in] T #DecisionTree to analyse
 * @param[in] data Analysis data
 */
static void decorator_score_sound_max(
    Hyperrectangle scores,
    const HyperrectangleDecorator x,
    const DecisionTree T,
    const AnalysisData data
) {
    const unsigned int n_labels = data->n_labels;
    DecisionTreeNode *L = data->L;
    unsigned int i, j, n_leaves;
    unsigned int * const local_scores = data->local_scores;

    reachable_leaves(L, &n_leaves, data->S, T, x->x);
    for (i = 0; i < n_labels; ++i) {
        local_scores[i] = 0;
    }

    for (j = 0; j < n_leaves; ++j) {
        const unsigned int *scores = ((struct node *) L[j]->data)->data.leaf.scores;
        const unsigned int max = ((struct node *) L[j]->data)->data.leaf.max_score;
        for (i = 0; i < n_labels; ++i) {
            if (scores[i] == max) {
                ++local_scores[i];
            }
        }
    }

    for (i = 0; i < n_labels; ++i) {
        scores->intervals[i].l += local_scores[i] == n_leaves;
        scores->intervals[i].u += local_scores[i] > 0;
    }
}



/**
 * Updates score of a decorator using the average voting scheme.
 *
 * Computes set of reachable leaves in a single tree and uses abstract
 * interpretation to overapproximate scores.
 *
 * @param[out] scores #Hyperrectangle of scores
 * @param[in] x Decorator to analyse
 * @param[in] T #DecisionTree to analyse
 * @param[in] data Analysis data
 */
static void decorator_score_sound_average(
    Hyperrectangle scores,
    const HyperrectangleDecorator x,
    const DecisionTree T,
    const AnalysisData data
) {
    const unsigned int n_labels = data->n_labels,
                       n_trees = data->n_trees;
    unsigned int i, j, n_leaves;
    DecisionTreeNode *L = data->L;
    Interval * const intervals = scores->intervals;

    reachable_leaves(L, &n_leaves, data->S, T, x->x);
    for (i = 0; i < n_labels; ++i) {
        double min = 1.0, max = 0.0;

        for (j = 0; j < n_leaves; ++j) {
            const unsigned int *scores = ((struct node *) L[j]->data)->data.leaf.scores,
                               n_samples = ((struct node *) L[j]->data)->data.leaf.n_samples;
            const double p = (double) scores[i] / (double) n_samples;

            if (p < min) {
                min = p;
            }
            if (p > max) {
                max = p;
            }
        }

        intervals[i].l += min / (double) n_trees;
        intervals[i].u += max / (double) n_trees;
    }
}



/**
 * Updates score of a decorator using the softargmax voting scheme.
 *
 * Computes set of reachable leaves in a single tree and uses abstract
 * interpretation to overapproximate scores.
 *
 * @param[out] scores #Hyperrectangle of scores
 * @param[in] x Decorator to analyse
 * @param[in] T #DecisionTree to analyse
 * @param[in] data Analysis data
 */
static void decorator_score_sound_softargmax(
    Hyperrectangle scores,
    const HyperrectangleDecorator x,
    const DecisionTree T,
    const AnalysisData data
) {
    const unsigned int n_labels = data->n_labels;
    unsigned int i, j, n_leaves;
    DecisionTreeNode *L = data->L;
    Interval * const intervals = scores->intervals;

    reachable_leaves(L, &n_leaves, data->S, T, x->x);
    for (i = 0; i < n_labels; ++i) {
        double min = +DBL_MAX, max = -DBL_MAX;

        for (j = 0; j < n_leaves; ++j) {
            const double *scores = ((struct node *) L[j]->data)->data.leaf_logarithmic.scores,
                         p = scores[i];

            if (p < min) {
                min = p;
            }
            if (p > max) {
                max = p;
            }
        }

        intervals[i].l += min;
        intervals[i].u += max;
    }
}



/**
 * Updates score of a decorator.
 *
 * Uses abstract interpretation to overapproximate intervals for scores
 * for given decorator. Voting scheme depends on the forest.
 *
 * @param[out] scores #Hyperrectangle of scores
 * @param[in] x Decorator to analyse
 * @param[in] data Analysis data
 */
static void decorator_score_sound(
    Hyperrectangle scores,
    const HyperrectangleDecorator x,
    const AnalysisData data
) {
    const unsigned int depth = decorator_get_depth(x),
                       n_trees = data->n_trees;
    const DecisionTree * const trees = forest_get_trees_as_array(data->F);
    void (*overapproximate)(Hyperrectangle, const HyperrectangleDecorator, const DecisionTree, const AnalysisData) = NULL;
    unsigned int i;

    switch (forest_get_voting_scheme(data->F)) {
    case FOREST_VOTING_MAX:
        overapproximate = decorator_score_sound_max;
        break;

    case FOREST_VOTING_AVERAGE:
        overapproximate = decorator_score_sound_average;
        break;

    case FOREST_VOTING_SOFTARGMAX:
        overapproximate = decorator_score_sound_softargmax;
        break;
    }

    for (i = depth; i < n_trees; ++i) {
        overapproximate(scores, x, trees[i], data);
    }

    if (forest_get_voting_scheme(data->F) == FOREST_VOTING_SOFTARGMAX) {
        const unsigned int n_labels = forest_get_n_labels(data->F);
        double s_min = 0.0, s_max = 0.0;
        for (i = 0; i < n_labels; ++i) {
            s_min += exp(scores->intervals[i].l);
            s_max += exp(scores->intervals[i].u);
        }
        for (i = 0; i < n_labels; ++i) {
            scores->intervals[i].l = exp(scores->intervals[i].l) / s_max;
            scores->intervals[i].u = exp(scores->intervals[i].u) / s_min;
        }
    }
}



/**
 * Computes an overapproximation of scores, as intervals.
 *
 * Uses concrete information from already reached leaves to compute a
 * guaranteed, partial score, then adds an overapproximation by using
 * abstract interpretation. Voting scheme depends on the forest.
 *
 * @param[out] scores #Hyperrectangle of scores
 * @param[in] x Decorator to analyse
 * @param[in] data Analysis data
 */
static void decorator_score_overapproximate(
    Hyperrectangle scores,
    const HyperrectangleDecorator x,
    const AnalysisData data
) {
    decorator_score_concrete(scores, x, data);
    decorator_score_sound(scores, x, data);
}



/**
 * Computes an overapproximation of set of labels of points in a decorator.
 *
 * Firs computes an overapproximation of scores, then determines which
 * labels have a maximal score.
 *
 * @param[out] labels #Set of labels
 * @param[in] x Decorator to analyse
 * @param[in] data Analysis data
 */
static void decorator_compute_labels(
    Set labels,
    const HyperrectangleDecorator x,
    const AnalysisData data
) {
    Hyperrectangle scores;

    hyperrectangle_create(&scores, data->n_labels);
    decorator_score_overapproximate(scores, x, data);
    scores_to_labels(labels, scores, data);
    hyperrectangle_delete(&scores);
}





/***********************************************************************
 * Internal functions and data structures.
 **********************************************************************/

/**
 * Enforces constraint that a attributes belonging to the same
 * categorical value must sum up to 1 and be either 0 or 1.
 *
 * @param[in] x Decorator to analyse
 * @param[in] tier Tier information
 * @param[in] i Index of attribute to check
 * @param[in] is_active Tells whether feature i was set on or off
 */
static void adjust_tier(Hyperrectangle x, const Tier tier, const unsigned int i, const unsigned is_active) {
    const unsigned int group = tier.tiers[i];

    /* Feature is not part of a tier */
    if (group == 0) {
        return;
    }

    /* If feature was activated, every feature in the same tier must be turned off */
    if (is_active) {
        unsigned int j;
        x->intervals[i].l = 1.0;
        x->intervals[i].u = 1.0;
        for (j = 0; j < tier.size; ++j) {
            if (j != i && tier.tiers[j] == group) {
                x->intervals[j].l = 0.0;
                x->intervals[j].u = 0.0;
            }
        }
    }

    /* If feature was turned of, and every other feature but one in the
       same tier is off, then the remaining feature must be turned on */
    else {
        unsigned int j, n_members = 0, n_off = 0, candidate = 0;
        x->intervals[i].l = 0.0;
        x->intervals[i].u = 0.0;
        for (j = 0; j < tier.size; ++j) {
            if (tier.tiers[j] == group) {
                const unsigned int is_off = x->intervals[j].l == 0.0 && x->intervals[j].u == 0.0;
                ++n_members;
                n_off += is_off;
                candidate = j * (1 - is_off);
            }
            if (n_members == n_off + 1) {
                x->intervals[candidate].l = 1.0;
                x->intervals[candidate].u = 1.0;
            }
        }
    }
}



/**
 * Tells whether an analysis is complete.
 *
 * An analysis is complete when a counterexample was discovered, or a
 * timeout was reached.
 *
 * @param[in] x Decorator to analyse
 * @param[in] context Analysis data
 * @return 1 if analysis must stop, 0 otherwise
 */
static unsigned int is_complete(const Node x, Context context) {
    (void) x;

    /* Stops if a counterexample is reached. */
    if (((struct analysis_data *) context)->internal_status != DONT_KNOW) {
        return 1;
    }


/*
    if (decorator_get_depth(x) > 25 && decorator_get_depth(x) < 45) {
        hyperrectangle_sample(((struct analysis_data *) context)->status->sample_b, ((const HyperrectangleDecorator) x)->x);
        forest_classify(((const HyperrectangleDecorator) x)->labels, ((struct analysis_data *) context)->F, ((struct analysis_data *) context)->status->sample_b);
        if (!set_is_equal(((const HyperrectangleDecorator) x)->labels, ((struct analysis_data *) context)->status->labels_a)) {
            ((struct analysis_data *) context)->internal_status = UNSTABLE;
            return 1;
        }
    }
*/

    /* Stops if a timeout was reached */
    if (time(NULL) - ((struct analysis_data *) context)->start_time > ((struct analysis_data *) context)->timeout) {
        ((struct analysis_data *) context)->internal_status = ABORTED;
        return 1;
    }

    return 0;
}



/**
 * Expands a decorator.
 *
 * Refines a decorator by partitioning it using a unexplored #DecisionTree.
 * For each reachable leaf in the next #DecisionTree, a new decorator is
 * created, storing information about its constraints, the new leaf, and
 * its parent decorator.
 *
 * If there are no more trees to explore, decorator cannot be refined and
 * its exploration terminates. Children which do not contribute to
 * analysis (i.e. their overapproximated set of labels is the same as the
 * labels of the original point) are not considered.
 *
 * @param[out] refined List of refined decorators
 * @param[in] n Decorator
 * @param[in,out] context Analysis data
 */
static void refine(List refined, const Node n, Context context) {
    const HyperrectangleDecorator x = (const HyperrectangleDecorator) n;
    struct analysis_data *data = (struct analysis_data *) context;
    const Forest F = data->F;
    StabilityStatus *status = data->status;
    const DecisionTree *trees = forest_get_trees_as_array(F);
    const unsigned int depth = decorator_get_depth(x);

    PriorityQueue Qx, Qt;

    /* No more trees for refinement: stops */
    if (depth == forest_get_n_trees(F)) {
        /* Decorator contains a counterexample */
        if (!set_is_equal(x->labels, data->status->labels_a)) {
            data->internal_status = UNSTABLE;
            hyperrectangle_midpoint(status->sample_b, x->x);
        }

        return;
    }


    /* Initializes data structures */
    priority_queue_create(&Qx);
    priority_queue_create(&Qt);

    Hyperrectangle y;
    hyperrectangle_create(&y, hyperrectangle_get_space_size(x->x));
    hyperrectangle_copy(y, x->x);

    priority_queue_push(Qx, y, 0.0);
    priority_queue_push(Qt, decision_tree_get_root(trees[depth]), 0.0);
    while (!priority_queue_is_empty(Qx)) {
        Hyperrectangle x_prime = priority_queue_pop(Qx);
        const DecisionTreeNode N = priority_queue_pop(Qt);
        unsigned int i;
        double k;
        const unsigned int depth = binary_tree_node_get_depth(N);

        /* A leaf was reached */
        if (decision_tree_node_is_leaf(N)) {
            HyperrectangleDecorator h;
            decorator_create(&h, x_prime, N, x);
            list_push(x->children, h);
            decorator_compute_labels(h->labels, h, data);

            /* Leaf contains a counterexample: stops */
            if (set_is_disjoint(h->labels, data->status->labels_a)) {
                data->internal_status = UNSTABLE;
                hyperrectangle_midpoint(status->sample_b, x_prime);
                break;
            }

            /* Leaf is "robust", does not help analysis: ignores */
            else if (set_is_equal(h->labels, data->status->labels_a)) {
                continue;
            }

            /* Leaf may contain a counterexample: adds to refined decorators */
            list_push(refined, h);
            continue;
        }


        /* An univariate linear split is reached */
        i = decision_tree_univariate_linear_split_get_index(N);
        k = decision_tree_univariate_linear_split_get_threshold(N);

        /* Hyperrectangle crosses cutting hyperplane: branches */
        if (x_prime->intervals[i].l <= k && x_prime->intervals[i].u > k) {
            Hyperrectangle x_right, x_left;
            double priority;

            x_left = x_prime;
            hyperrectangle_create(&x_right, hyperrectangle_get_space_size(x_prime));
            hyperrectangle_copy(x_right, x_prime);

            x_left->intervals[i].u = min(x_left->intervals[i].u, k);
            adjust_tier(x_left, data->tier, i, 0);
            priority = depth + (k - x_prime->intervals[i].l) / interval_radius(x_prime->intervals[i]);
            priority_queue_push(Qx, x_left, priority);
            priority_queue_push(Qt, decision_tree_univariate_linear_split_get_left_child(N), priority);

            x_right->intervals[i].l = max(x_left->intervals[i].u, k + EPSILON);
            adjust_tier(x_right, data->tier, i, 1);
            priority = depth + (x_prime->intervals[i].u - k) / interval_radius(x_prime->intervals[i]);
            priority_queue_push(Qx, x_right, priority);
            priority_queue_push(Qt, decision_tree_univariate_linear_split_get_right_child(N), priority);
        }

        /* Hyperrectangle belongs to left hyperspace */
        else if (x_prime->intervals[i].u <= k) {
            adjust_tier(x_prime, data->tier, i, 0);
            double priority = depth + (k - x_prime->intervals[i].l) / interval_radius(x_prime->intervals[i]);
            priority_queue_push(Qx, x_prime, priority);
            priority_queue_push(Qt, decision_tree_univariate_linear_split_get_left_child(N), priority);
        }

        /* Hyperrectangle belongs to right hyperspace */
        else if (x_prime->intervals[i].l > k) {
            adjust_tier(x_prime, data->tier, i, 1);
            double priority = depth + (x_prime->intervals[i].u - k) / interval_radius(x_prime->intervals[i]);
            priority_queue_push(Qx, x_prime, priority);
            priority_queue_push(Qt, decision_tree_univariate_linear_split_get_right_child(N), priority);
        }
    }


    /* Deallocates memory */
    while (!priority_queue_is_empty(Qx)) {
        Hyperrectangle x = priority_queue_pop(Qx);
        hyperrectangle_delete(&x);
        priority_queue_pop(Qt);
    }
    priority_queue_delete(&Qx);
    priority_queue_delete(&Qt);
    hyperrectangle_delete(&x->x);
    x->x = NULL;
}



/**
 * Estimates priority of a decorator during a best-first search.
 *
 * Uses a heuristics to estimate utility of a decorator.
 *
 * @param[in] x Decorator
 * @param[in] context Analysis data
 * @return Estimated priority
 */
static double compute_priority(const Node x, Context context) {
    const HyperrectangleDecorator h = (const HyperrectangleDecorator) x;
    const struct analysis_data *data = (const struct analysis_data *) context;
    const Set abstract_labels = data->local_labels;

    const double volume = hyperrectangle_volume(h->x),
                 n_labels_l = set_get_cardinality(abstract_labels),
                 depth = decorator_get_depth(h);

    set_intersection(abstract_labels, abstract_labels, data->status->labels_a);
    const double intersection_size = set_get_cardinality(abstract_labels);

    return
        - 1e6 * volume
        + 1.0 * depth
        + 1.0 * (n_labels_l - intersection_size) / data->n_labels
        ;
}





/***********************************************************************
 * Public functions.
 **********************************************************************/

void forest_hyperrectangle_is_stable(
    StabilityStatus *status,
    const Forest F,
    const Hyperrectangle x,
    const Tier t
) {
    Hyperrectangle x_prime;
    HyperrectangleDecorator start, goal;
    struct analysis_data data;
    const unsigned int has_sample = status->has_sample;

    /* Ensures presence of a sample */
    if (!has_sample) {
        hyperrectangle_midpoint(status->sample_a, x);
        set_create(&status->labels_a, set_equality_string);
        forest_classify(status->labels_a, F, status->sample_a);
    }

    /* Initializes data strucutres */
    hyperrectangle_create(&x_prime, hyperrectangle_get_space_size(x));
    hyperrectangle_copy(x_prime, x);
    decorator_create(&start, x_prime, NULL, NULL);
    data.status = status;
    data.F = F;
    data.start_time = time(NULL);
    data.timeout = status->timeout;
    data.internal_status = DONT_KNOW;
    data.labels = forest_get_labels_as_array(F);
    data.n_labels = forest_get_n_labels(F);
    data.n_trees = forest_get_n_trees(F);
    data.S = malloc(CONTAINER_SIZE * sizeof(DecisionTreeNode));
    data.L = malloc(CONTAINER_SIZE * sizeof(DecisionTreeNode));
    data.local_scores = (unsigned int *) malloc(forest_get_n_labels(F) * sizeof(unsigned int));
    set_create(&data.local_labels, set_equality_string);
    data.tier = t;


    /* Runs analysis */
    best_first_search((Node *) &goal, start, is_complete, refine, compute_priority, &data);


    /* Checks result */
    switch (data.internal_status) {
    case DONT_KNOW:
        status->result = STABILITY_TRUE;
        break;

    case UNSTABLE:
        status->result = STABILITY_FALSE;
        break;

    case ABORTED:
        status->result = STABILITY_DONT_KNOW;
        break;
    }


    /* Deallocates memory */
    if (!has_sample) {
        stability_status_unset_sample(status);
    }
    decorator_delete(&start);
    free(data.S);
    free(data.L);
    free(data.local_scores);
    set_delete(&data.local_labels);
}
