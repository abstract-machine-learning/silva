/**
 * A priority queue.
 * 
 * @file priority_queue.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdio.h>

/** Type of priority queue. */
typedef struct priority_queue *PriorityQueue;


/** Type of a priority queue printer. */
typedef void (*PriorityQueuePrinter)(const void *, FILE *);


/**
 * Creates a priority queue.
 *
 * @param[out] P Priority queue
 * @warning #priority_queue_delete should be called to ensure proper
 *          memory deallocation.
 */
void priority_queue_create(PriorityQueue *P);


/**
 * Deletes a priority queue.
 *
 * @param[out] P Priority queue
 */
void priority_queue_delete(PriorityQueue *P);



/**
 * Tells whether a priority queue is empty.
 *
 * @param[in] P Priority queue
 * @return 1 if priority queue is empty, 0 otherwise
 */
unsigned int priority_queue_is_empty(const PriorityQueue P);



/**
 * Returns number of elements in a priority queue.
 *
 * @param[in] P Priority queue
 * @return Number of elements in the priority queue
 */
unsigned int priority_queue_get_size(const PriorityQueue P);


/**
 * Returns priority of element with highest priority.
 *
 * @param[in] P Priority queue
 * @return Priority of element with highest priority
 */
double priority_queue_get_max_priority(const PriorityQueue P);


/**
 * Returns element with highest priority.
 *
 * @param[in] P Priority queue
 * @return Element with highest priority
 */
void *priority_queue_peek(const PriorityQueue P);



/**
 * Adds an element into a priority queue.
 *
 * @param[in,out] P Priority queue
 * @param[in] x Element to add
 * @param[in] priority Priority value
 */
void priority_queue_push(PriorityQueue P, const void *x, const double priority);


/**
 * Removes and returns element with highest priority from a priority queue.
 *
 * @param[out] P Priority queue
 * @return Element with highest priority
 */
void *priority_queue_pop(PriorityQueue P);



/**
 * Prints a priority queue.
 *
 * @param[in] P Priority queue
 * @param[in] printer Priority queue printer
 * @param[out] stream Stream
 */
void priority_queue_print(
    const PriorityQueue P,
    const PriorityQueuePrinter printer,
    FILE *stream
);

#endif
