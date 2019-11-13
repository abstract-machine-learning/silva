/**
 * Implements a generic list.
 *
 * @file list.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "list.h"

#include <stdlib.h>
#include <string.h>


/** Default initial size of a list. */
#define LIST_SIZE 32


/** Structure of a list. */
struct list {
    void **elements;        /**< Array of elements. */
    unsigned int from;      /**< Index of first element. */
    unsigned int to;        /**< Index of last element plus one. */
    unsigned int capacity;  /**< Maximum number of elements. */
};



/***********************************************************************
 * Internal support functions.
 **********************************************************************/

/**
 * Resizes capacity of a list and reset position of elements.
 *
 * @param[out] L List
 * @param[in] size New capacity
 * @warning If new size is too small to contain old elements, the latter
 *          are lost.
 */
static void list_resize(List L, const unsigned int size) {
    if (L->from != 0) {
        memmove(
            (void *) L->elements,
            (void *) (L->elements + L->from),
            (L->to - L->from) * sizeof(void *)
        );
        L->to -= L->from;
        L->from = 0;
    }

    if (size != L->capacity) {
        L->elements = (void **) realloc((void *) L->elements, size * sizeof(void *));
        if (L->to > size) {
            L->to = size + 1;
        }
        L->capacity = size;
    }
}





/***********************************************************************
 * Public functions.
 **********************************************************************/
void list_create(List *L) {
    List l = (List) malloc(sizeof(struct list));
    if (l == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    l->elements = (void **) malloc(LIST_SIZE * sizeof(void *));
    if (l->elements == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    l->from = 0;
    l->to = 0;
    l->capacity = LIST_SIZE;

    *L = l;
}



void list_delete(List *L) {
    if (L == NULL || *L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    free((*L)->elements);
    free(*L);
    *L = NULL;
}



void list_from_array(List L, const void **array, const unsigned int n_elements) {
    if (L == NULL || array == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    if (n_elements > L->capacity) {
        list_resize(L, n_elements);
    }

    memcpy((void *) L->elements, (void *) array, n_elements * sizeof(void *));
    L->from = 0;
    L->to = n_elements;
}



void list_wrap_array(List L, void ** const array, const unsigned int n_elements) {
    if (L == NULL || array == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    free(L->elements);
    L->elements = array;
    L->from = 0;
    L->to = n_elements;
    L->capacity = n_elements;
}



unsigned int list_is_empty(const List L) {
    if (L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return L->to == L->from;
}



unsigned int list_get_size(const List L) {
    if (L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return L->to - L->from;
}



void *list_get_at(const List L, const unsigned int i) {
    if (L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    if (i > L->to - L->from) {
        fprintf(stderr, "[%s: %d] Index out of bound.\n", __FILE__, __LINE__);
        abort();
    }

    return L->elements[L->from + i];
}



void *list_get_head(const List L) {
    return list_get_at(L, 0);
}



void *list_get_last(const List L) {
    if (L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    if (L->to == L->from) {
        fprintf(stderr, "[%s: %d] Index out of bound.\n", __FILE__, __LINE__);
        abort();
    }

    return L->elements[L->to - 1];
}



void list_to_array(void **array, const List L) {
    if (L == NULL || array == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    memcpy(
        (void *) array,
        (void *) (L->elements + L->from),
        (L->to - L->from) * sizeof(void *)
    );
}



void **list_as_array(const List L) {
    if (L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return L->elements + L->from;
}



void list_append(List L, void * const x) {
    if (L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    if (L->to - L->from == L->capacity) {
        list_resize(L, 2 * L->capacity);
    }

    L->elements[L->to] = x;
    ++L->to;
}



void list_prepend(List L, void * const x) {
    if (L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    if (L->from == 0) {
        if (L->to == L->capacity) {
            list_resize(L, 2 * L->capacity);
        }
            
        memmove(
            (void *) (L->elements + L->from + 1),
            (void *) (L->elements + L->from),
            (L->to - L->from) * sizeof(void *)
        );
        ++L->to;
    }
    else {
        --L->from;
    }

    L->elements[L->from] = x;
}



void list_push(List L, void * const x) {
    list_append(L, x);
}



void list_unshift(List L, void * const x) {
    list_prepend(L, x);
}



void *list_shift(List L) {
    if (L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    if (L->to == L->from) {
        fprintf(stderr, "[%s: %d] Index out of bound.\n", __FILE__, __LINE__);
        abort();
    }

    ++L->from;
    return L->elements[L->from - 1];
}



void *list_pop(List L) {
    if (L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    if (L->to == L->from) {
        fprintf(stderr, "[%s: %d] Index out of bound.\n", __FILE__, __LINE__);
        abort();
    }

    --L->to;
    return L->elements[L->to];
}



void list_print(const List L, const ListPrinter printer, void * const data, FILE * const stream) {
    unsigned int i;

    if (L == NULL) {
        fprintf(stream, "NULL");
        return;
    }

    fprintf(stream, "List at @%p, with %u elements: [", (void *) L, L->to - L->from);
    for (i = L->from; i < L->to; ++i) {
        if (printer) {
            printer(L->elements[i], data, stream);
        }
        else {
            fprintf(stream, "%p", L->elements[i]);
        }

        if (i + 1 < L->to) {
            fprintf(stream, ", ");
        }
    }
    fprintf(stream, "]");
}





/***********************************************************************
 * List iterator functions.
 **********************************************************************/

ListIterator list_begin(const List L) {
    if (L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return L->elements + L->from;
}



ListIterator list_last(const List L) {
    if (L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return L->elements + (L->to != L->from ? L->to - 1 : L->from);
}



ListIterator list_end(const List L) {
    if (L == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return L->elements + L->to;
}
