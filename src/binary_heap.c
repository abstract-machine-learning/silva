/**
 * A binary heap.
 *
 * @file binary_heap.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "binary_heap.h"

#include <stdlib.h>


/** Default initial heap capacity. */
#define DEFAULT_INITIAL_CAPACITY 0x10

/**
 * Computes index of parent node.
 *
 * @param[in] i Index of node
 * @return Index of parent node
 */
#define index_of_parent(i) (((i) - 1) >> 1)


/**
 * Computes index of left child.
 *
 * param[in] index Index of node
 * @return Index of left child
 */
#define index_of_left_child(i) (((i) << 1) + 1)


/**
 * Computes index of right child.
 *
 * param[in] index Index of node
 * @return Index of right child
 */
#define index_of_right_child(i) (((i) << 1) + 2)



/** Structure of a node: \f$\langle element, key \rangle\f$. */
struct node {
    void *data;  /**< Value of the element. */
    double key;  /**< Numberical key of the element. */
};

/** Type of a node. */
typedef struct node Node;


/**
 * Structure of a binary heap.
 */
struct binary_heap {
    Node *nodes;            /**< Nodes in the binary heap. */
    unsigned int size;      /**< Number of nodes in the binary heap. */
    unsigned int capacity;  /**< Maximum number of nodes in the binary heap. */
    HeapType type;          /**< Type of heap. */
};



/***********************************************************************
 * Internal functions.
 **********************************************************************/

/**
 * Swaps two nodes in an array.
 *
 * @param[in,out] nodes Array of nodes
 * @param[in] i Index of first node
 * @param[in] j Index of second node
 */
static void swap_nodes(Node *nodes, const unsigned int i, const unsigned int j) {
    Node swap = nodes[i];
    nodes[i] = nodes[j];
    nodes[j] = swap;
}



/**
 * Tells whether a node and its parent satisfy the heap property.
 *
 * @param[in] H Binary heap
 * @param[in] node_idx Index of the node
 * @return 1 if heap property holds, 0 otherwise
 */
static unsigned int heap_property_local(
    const BinaryHeap H,
    const unsigned int node_idx
) {
    if (node_idx == 0) {
        return 1;
    }

    switch (H->type) {
        case HEAP_MIN:
            return H->nodes[index_of_parent(node_idx)].key <= H->nodes[node_idx].key;
        case HEAP_MAX:
            return H->nodes[index_of_parent(node_idx)].key >= H->nodes[node_idx].key;
    }

    return 0;
}



/**
 * Choses child to swap with in a sift down.
 *
 * @param[in] H Binary heap
 * @param[in] left Index of left child
 * @param[in] right Index of right child
 * @return Index of child to swap with
 */
static unsigned int chose_child(
    const BinaryHeap H,
    const unsigned int left,
    const unsigned int right
) {
    switch (H->type) {
        case HEAP_MIN:
            return H->nodes[left].key < H->nodes[right].key ? left : right;
        case HEAP_MAX:
            return H->nodes[left].key > H->nodes[right].key ? left : right;
    }

    return left;
}



/**
 * Restores a binary heap by sifting up an element.
 *
 * @param[in,out] H Binary heap to restore
 * @param[in] index Index of element to sift up
 */
static void sift_up(BinaryHeap H, const unsigned int index) {
    unsigned int node_idx = index;

    while (!heap_property_local(H, node_idx)) {
        const unsigned int parent_idx = index_of_parent(node_idx);
        swap_nodes(H->nodes, node_idx, parent_idx);
        node_idx = parent_idx;
    }
}



/**
 * Restores a binary heap by sifting down an element.
 *
 * @param[in,out] H Binary heap to restore
 * @param[in] index Index of element to sift up
 */
static void sift_down(BinaryHeap H, const unsigned int index) {
    unsigned int node_idx = index, is_restored = 0;

    while (1) {
        const unsigned int left_idx = index_of_left_child(node_idx),
                           right_idx = index_of_right_child(node_idx);
        unsigned int swap_idx;

        is_restored = 1;
        if (left_idx < H->size) {
            is_restored &= heap_property_local(H, left_idx);
        }
        if (right_idx < H->size) {
            is_restored &= heap_property_local(H, right_idx);
        }

        if (is_restored) {
            break;
        }
        swap_idx = chose_child(H, left_idx, right_idx);
        swap_nodes(H->nodes, node_idx, swap_idx);
        node_idx = swap_idx;
    }
}



/**
 * Recursively prints a binary heap.
 *
 * @param[in] H Binary heap
 * @param[in] printer Binary heap printer
 * @param[out] stream Stream
 * @param[in] node_idx Index of node to print
 * @param[in] indentation Indentation level
 */
static void print_indented(
    const BinaryHeap H,
    const BinaryHeapPrinter printer,
    FILE *stream,
    const unsigned int node_idx,
    const unsigned int indentation
) {
    unsigned int i;

    /* Base case: no node to print */
    if (node_idx >= H->size) {
        return;
    }

    /* Inductive case: prints spacing, node and children */
    fprintf(stream, "\n");
    for (i = 0; i < indentation; ++i) {
        fprintf(stream, "\t");
    }
    fprintf(stream, "(");
    if (printer) {
        printer(H->nodes[node_idx].data, stream);
    }
    else {
        fprintf(stream, "%p", (void *) H->nodes[node_idx].data);
    }
    fprintf(stream, ", %g)", H->nodes[node_idx].key);
    print_indented(H, printer, stream, index_of_left_child(node_idx), indentation + 1);
    print_indented(H, printer, stream, index_of_right_child(node_idx), indentation + 1);
}



/***********************************************************************
 * Public functions.
 **********************************************************************/

void binary_heap_create(BinaryHeap *H, const HeapType type) {
    BinaryHeap h = (BinaryHeap) malloc(sizeof(struct binary_heap));

    h->nodes = (Node *) malloc(DEFAULT_INITIAL_CAPACITY * sizeof(Node));
    h->size = 0;
    h->capacity = DEFAULT_INITIAL_CAPACITY;
    h->type = type;

    *H = h;
}



void binary_heap_delete(BinaryHeap *H) {
    free((*H)->nodes);
    free(*H);
    *H = NULL;
}



unsigned int binary_heap_is_empty(const BinaryHeap H) {
    return H ? H->size == 0 : 1;
}



unsigned int binary_heap_get_size(const BinaryHeap H) {
    return H ? H->size : 0;
}



double binary_heap_get_next_key(const BinaryHeap H) {
    return H ? H->nodes[0].key : 0.0;
}



void *binary_heap_peek(const BinaryHeap H) {
    return H ? H->nodes[0].data : NULL;
}



void binary_heap_push(BinaryHeap H, const void *x, const double key) {
    /* Increases heap capacity if full */
    if (H->size == H->capacity) {
        H->capacity *= 2;
        H->nodes = (Node *) realloc(H->nodes, H->capacity * sizeof(Node));
    }

    /* Inserts element */
    H->nodes[H->size].data = (void *) x;
    H->nodes[H->size].key = key;
    sift_up(H, H->size);
    ++H->size;
}



void *binary_heap_pop(BinaryHeap H) {
    void *x;

    if (H->size == 0) {
        fprintf(stderr, "[%s: %d] Trying to pop from an empty binary heap.\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    x = H->nodes[0].data;
    H->nodes[0] = H->nodes[H->size - 1];
    --H->size;
    sift_down(H, 0);

    return x;
}



void binary_heap_print(
    const BinaryHeap H,
    const BinaryHeapPrinter printer,
    FILE *stream
) {
    fprintf(stream, "Binary heap @%p, of size %u:", (void *) H, H->size);
    print_indented(H, printer, stream, 0, 0);
    fprintf(stream, "\n");
}
