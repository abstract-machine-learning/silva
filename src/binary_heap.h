/**
 * A binary heap.
 *
 * @file binary_heap.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <stdio.h>

/** Types of heap. */
typedef enum {
    HEAP_MIN,  /**< A min-heap. */
    HEAP_MAX   /**< A max-heap. */
} HeapType;


/** Type of a binary heap. */
typedef struct binary_heap *BinaryHeap;


/** Type of a binary heap printer. */
typedef void (*BinaryHeapPrinter)(const void *, FILE *);



/**
 * Creates a binary heap.
 *
 * @param[out] H Binary heap
 * @param[in] type Type of heap
 * @warning #binary_heap_delete should be called to esnure proper memory
 *          deallocation.
 */
void binary_heap_create(BinaryHeap *H, const HeapType type);


/**
 * Deletes a binary heap.
 *
 * @param[out] H Binary heap
 */
void binary_heap_delete(BinaryHeap *H);



/**
 * Tells whether a binary heap is empty.
 *
 * @param[in] H Binary heap
 * @return 1 if binary heap is empty, 0 otherwise
 */
unsigned int binary_heap_is_empty(const BinaryHeap H);



/**
 * Returns number of elements in a binary heap.
 *
 * @param[in] H Binary heap
 * @return Number of elements in the binary heap
 */
unsigned int binary_heap_get_size(const BinaryHeap H);


/**
 * Returns key of first element in a binary heap.
 *
 * @param[in] H Binary heap
 * @return Key of first element in the binary heap
 */
double binary_heap_get_next_key(const BinaryHeap H);


/**
 * Returns first element in a binary heap.
 *
 * @param[in] H Binary heap
 * @return First element in the binary heap
 */
void *binary_heap_peek(const BinaryHeap H);



/**
 * Adds an element into a binary heap.
 * 
 * @param[in,out] H Binary heap
 * @param[in] x Element to add
 * @param[in] key Numerical key of the element
 */
void binary_heap_push(BinaryHeap H, const void *x, const double key);


/**
 * Removes and returns first element from a binary heap.
 *
 * @param[out] H Binary heap
 * @return First element in the heap
 */
void *binary_heap_pop(BinaryHeap H);



/**
 * Prints a binary heap.
 *
 * @param[in] H Binary heap
 * @param[in] printer Binary heap printer
 * @param[out] stream Stream
 */
void binary_heap_print(
    const BinaryHeap H,
    const BinaryHeapPrinter printer,
    FILE *stream
);

#endif
