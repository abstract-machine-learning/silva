/**
 * Defines a generic stack.
 *
 * @file stack.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef STACK_H
#define STACK_H

#include <stdio.h>

/** Type of a stack. */
typedef struct stack *Stack;


/** Type of a stack printer. */
typedef void (*StackPrinter)(const void *, FILE *);


/**
 * Creates an empty stack.
 *
 * @param[out] S Pointer to stack to create
 * @warning #stack_delete should be called to ensure proper memory deallocation
 */
void stack_create(Stack *S);


/**
 * Deletes a stack.
 *
 * @param[out] S Pointer to stack to delete
 */
void stack_delete(Stack *S);


/**
 * Tells whether a stack is empty.
 *
 * @param[in] S Stack
 * @return 1 if S is empty, 0 otherwise
 */
unsigned int stack_is_empty(const Stack S);


/**
 * Returns number of elements in a stack.
 *
 * @param[in] S Stack
 * @return Number of elements in the S
 */
unsigned int stack_get_size(const Stack S);


/**
 * Returns element on top of stack without removing it.
 *
 * @param[in] S Stack
 * @return Element on top of S
 */
void *stack_peek(const Stack S);


/**
 * Pops element on top of stack.
 *
 * @param[out] S Stack
 * @return Element on top of S
 */
void *stack_pop(Stack S);


/**
 * Pushes an element into a stack.
 *
 * @param[out] S Stack
 * @param[in] element Element to push into the S
 */
void stack_push(Stack S, const void *element);


/**
 * Prints a stack.
 *
 * @param[in] S Stack to print
 * @param[out] stream Stream
 * @param[in] printer Stack printer, or NULL
 */
void stack_print(
    const Stack S,
    FILE *stream,
    const StackPrinter printer
);

#endif
