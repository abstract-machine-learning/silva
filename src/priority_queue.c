/**
 * A priority queue.
 * 
 * @file priority_queue.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "priority_queue.h"

#include <stdlib.h>

#include "binary_heap.h"


/** Structure of a priority queue. */
struct priority_queue {
    BinaryHeap heap;  /**< Binary heap. */
};



void priority_queue_create(PriorityQueue *P) {
    PriorityQueue p = (PriorityQueue) malloc(sizeof(struct priority_queue));
    binary_heap_create(&p->heap, HEAP_MAX);

    *P = p;
}



void priority_queue_delete(PriorityQueue *P) {
    binary_heap_delete(&(*P)->heap);
    free(*P);
    *P = NULL;
}



unsigned int priority_queue_is_empty(const PriorityQueue P) {
    return binary_heap_is_empty(P->heap);
}



unsigned int priority_queue_get_size(const PriorityQueue P) {
    return binary_heap_get_size(P->heap);
}



double priority_queue_get_max_priority(const PriorityQueue P) {
    return binary_heap_get_next_key(P->heap);
}



void *priority_queue_peek(const PriorityQueue P) {
    return binary_heap_peek(P->heap);
}



void priority_queue_push(PriorityQueue P, const void *x, const double priority) {
    binary_heap_push(P->heap, x, priority);
}



void *priority_queue_pop(PriorityQueue P) {
    return binary_heap_pop(P->heap);
}



void priority_queue_print(
    const PriorityQueue P,
    const PriorityQueuePrinter printer,
    FILE *stream
) {
    fprintf(stream, "Priority queue @%p, uses the following binary heap:\n", (void *) P);
    binary_heap_print(P->heap, (BinaryHeapPrinter) printer, stream);
}
