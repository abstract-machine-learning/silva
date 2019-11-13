/**
 * Implements generic stack.
 *
 * @file stack.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "stack.h"

#include <stdlib.h>


/** Default initial capacity. */
#define INITIAL_CAPACITY 1024


/** Structure of a stack. */
struct stack {
    void **elements;          /**< Array of elements in the stack. */
    unsigned int n_elements;  /**< Number of elements in the stack. */
    unsigned int capacity;    /**< Maximum number of elements in the stack. */
};



void stack_create(Stack *S) {
    Stack s = (Stack) malloc(sizeof(struct stack));
    if (s == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    s->elements = (void **) malloc(INITIAL_CAPACITY * sizeof(void *));
    if (s->elements == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    s->n_elements = 0;
    s->capacity = INITIAL_CAPACITY;

    *S = s;
}



void stack_delete(Stack *S) {
    if (S == NULL || *S == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    free((*S)->elements);
    free(*S);
    *S = NULL;
}



unsigned int stack_is_empty(const Stack S) {
    if (S == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return S->n_elements == 0;
}



unsigned int stack_get_size(const Stack S) {
    if (S == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return S->n_elements;
}



void *stack_peek(const Stack S) {
    if (stack_is_empty(S)) {
        fprintf(stderr, "[%s: %d] Trying to peek from an empty stack.\n", __FILE__, __LINE__);
        abort();
    }

    return S->elements[S->n_elements - 1];
}



void *stack_pop(Stack S) {
    void *data;

    if (stack_is_empty(S)) {
        fprintf(stderr, "[%s: %d] Trying to pop from an empty stack.\n", __FILE__, __LINE__);
        abort();
    }

    data = S->elements[S->n_elements - 1];
    --S->n_elements;

    return data;
}



void stack_push(Stack S, const void *element) {
    if (S == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    if (S->n_elements == S->capacity) {
        S->capacity *= 2;
        S->elements = (void **) realloc(S->elements, S->capacity * sizeof(void *));
    }

    S->elements[S->n_elements] = (void *) element;
    ++S->n_elements;
}



void stack_print(
    const Stack S,
    FILE *stream,
    const StackPrinter printer
) {
    unsigned int i;

    if (!S) {
        fprintf(stream, "NULL\n");
    }

    fprintf(stream, "Stack @%p, %u elements:\n", (void *) S, S->n_elements);
    for (i = 0; i < S->n_elements; ++i) {
        if (printer) {
            printer(S->elements[i], stream);
        }
        else {
            fprintf(stream, "%p ", S->elements[i]);
        }
    }
    fprintf(stream, "\n");
}
