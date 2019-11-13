/**
 * A generic set.
 *
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 * @file set.c
 */
#include "set.h"

#include <stdlib.h>
#include <string.h>


/** Default initial capacity of a set. */
#define SET_INITIAL_CAPACITY 16



/** Structure of a set. */
struct set {
    unsigned int capacity;        /**< Maximum number of elements. */
    unsigned int size;            /**< Number of elements. */
    void **elements;              /**< Elements. */
    SetElementEquality equality;  /**< Equality operator over elements. */
};



/***********************************************************************
 * Internal functions.
 **********************************************************************/

/**
 * Default equality test: pointer equality.
 *
 * @param[in] x First pointer
 * @param[in] y Second pointer
 * @return 1 if x is equal to y, 0 otherwise
 */
static unsigned int default_equality(const void *x, const void *y) {
    return x == y;
}



/***********************************************************************
 * Public functions.
 **********************************************************************/

void set_create(Set *S, SetElementEquality equality) {
    *S = (Set) malloc(sizeof(struct set));

    (*S)->capacity = SET_INITIAL_CAPACITY;
    (*S)->size = 0;
    (*S)->elements = (void **) malloc(SET_INITIAL_CAPACITY * sizeof(void *));
    (*S)->equality = equality != NULL ? equality : default_equality;
}



void set_delete(Set *S) {
    if (S == NULL || *S == NULL) {
        return;
    }

    free((*S)->elements);
    free(*S);
    *S = NULL;
}



unsigned int set_is_empty(const Set S) {
    return !S || S->size == 0 ? 1 : 0;
}



unsigned int set_is_singleton(const Set S) {
    return S ? S->size == 1 : 0;
}



unsigned int set_has_element(const Set S, const void *x) {
    unsigned int i;

    if (S == NULL) {
        return 0;
    }

    for (i = 0; i < S->size; ++i) {
        if (S->equality(x, S->elements[i])) {
            return 1;
        }
    }

    return 0;
}



unsigned int set_is_subset(const Set A, const Set B) {
    unsigned int i;

    if (A == NULL) {
        return 1;
    }

    for (i = 0; i < A->size; ++i) {
        if (!set_has_element(B, A->elements[i])) {
            return 0;
        }
    }

    return 1;
}



unsigned int set_is_proper_subset(const Set A, const Set B) {
    unsigned int i, n_common = 0;

    if (A == NULL) {
        return !set_is_empty(B);
    }

    for (i = 0; i < A->size; ++i) {
        if (!set_has_element(B, A->elements[i])) {
            return 0;
        }

        ++n_common;
    }

    return n_common != A->size;
}



unsigned int set_is_superset(const Set A, const Set B) {
    return set_is_subset(B, A);
}



unsigned int set_is_equal(const Set A, const Set B) {
    return A->size == B->size ? set_is_subset(A, B) && set_is_subset(B, A) : 0;
}



unsigned int set_is_disjoint(const Set A, const Set B) {
    unsigned int i, j;

    if (A == NULL || B == NULL) {
        return 1;
    }

    for (i = 0; i < A->size; ++i) {
        for (j = 0; j < B->size; ++j) {
            if (A->equality(A->elements[i], B->elements[j])) {
                return 0;
            }
        }
    }

    return 1;
}



unsigned int set_forall(const Set S, const SetProperty P, void *data) {
    unsigned int i;

    for (i = 0; i < set_get_cardinality(S); ++i) {
        const void *x = S->elements[i];
        if (!P(x, data)) {
            return 0;
        }
    }

    return 1;
}



unsigned int set_exists(const Set S, const SetProperty P, void *data) {
    unsigned int i;

    for (i = 0; i < set_get_cardinality(S); ++i) {
        const void *x = S->elements[i]; 
        if (P(x, data)) {
            return 1;
        }
    }

    return 0;
}



unsigned int set_get_cardinality(const Set S) {
    return S != NULL ? S->size : 0;
}



void *set_get_elements_as_array(const Set S) {
    return S != NULL ? S->elements : NULL;
}



void set_get_subset(Set R, const Set S, const SetProperty P, void *data) {
    Set T;
    unsigned int i;

    if (S == NULL) {
        set_clear(R);
        return;
    }

    /* Ensures that T and S are not the same pointer and that T is empty */
    if (R != S) {
        T = R;
        set_clear(T);
    }
    else {
        set_create(&T, S->equality);
    }

    /* Adds to T elements satisfying P */
    for (i = 0; i < S->size; ++i) {
        const void *x = S->elements[i];
        if (P(x, data)) {
            set_add_element(T, x);
        }
    }

    /* If T was a new set, it is copied to R */
    if (T != R) {
        set_copy(R, T);
        set_delete(&T);
    }
}



void set_copy(Set R, const Set S) {
    if (S == NULL || R == NULL || R == S) {
        return;
    }

    R->capacity = S->capacity;
    R->size = S->size;
    R->elements = (void **) realloc(R->elements, S->capacity * sizeof(void *));
    memcpy(R->elements, S->elements, S->size * sizeof(void *));
    R->equality = S->equality;
}



void set_clear(Set S) {
    if (S == NULL) {
        return;
    }

    S->size = 0;
}



void set_add_element(Set S, const void *x) {
    if (S == NULL) {
        fprintf(stderr, "[%s: %d] NULL pointer to set.\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    if (set_has_element(S, x)) {
        return;
    }

    if (S->size == S->capacity) {
        S->capacity *= 2;
        S->elements = (void **) realloc(S->elements, S->capacity * sizeof(void *));
    }

    S->elements[S->size] = (void *) x;
    ++S->size;
}



void set_remove_element(Set S, const void *x) {
    unsigned int i;

    if (S == NULL) {
        return;
    }

    for (i = 0; i < S->size; ++i) {
        if (S->equality(S->elements[i], x)) {
            if (i < S->size - 1) {
                memmove(S->elements + i, S->elements + i + 1, (S->size - i - 1) * sizeof(void *));
            }
            --S->size;
        }
    }
}



void set_intersection(Set R, const Set A, const Set B) {
    Set T;
    unsigned int i;

    /* Ensures that T and A and B are not the same pointers, and that T is empty */
    if (R == A || R == B) {
        set_create(&T, A->equality);
    }
    else {
        T = R;
        set_clear(T);
    }

    /* Adds to T elements common between A and B */
    for (i = 0; i < set_get_cardinality(A); ++i) {
        const void *x = A->elements[i];
        if (set_has_element(B, x)) {
            set_add_element(T, x);
        }
    }

    /* If T is a new set, it is copied back to R */
    if (T != R) {
        set_copy(R, T);
        set_delete(&T);
    }
}



void set_union(Set R, const Set A, const Set B) {
    Set T;
    unsigned int i;

    /* Ensures that T and A and B are not the same pointers, and that T is empty */
    if (R == A || R == B) {
        set_create(&T, A->equality);
    }
    else {
        T = R;
        set_clear(T);
    }

    /* Adds to T elements belonging to either A or B */
    for (i = 0; i < set_get_cardinality(A); ++i) {
        const void *x = A->elements[i];
        set_add_element(T, x);
    }
    for (i = 0; i < set_get_cardinality(B); ++i) {
        const void *x = B->elements[i];
        set_add_element(T, x);
    }

    /* If T is a new set, it is copied back to R */
    if (T != R) {
        set_copy(R, T);
        set_delete(&T);
    }
}



void set_difference(Set R, const Set A, const Set B) {
    Set T;
    unsigned int i;

    /* Ensures that T and A and B are not the same pointers, and that T is empty */
    if (R == A || R == B) {
        set_create(&T, A->equality);
    }
    else {
        T = R;
        set_clear(T);
    }

    /* Adds to T elements in A not belongin to B */
    for (i = 0; i < set_get_cardinality(A); ++i) {
        const void *x = A->elements[i];
        if (!set_has_element(B, x)) {
            set_add_element(T, x);
        }
    }

    /* If T is a new set, it is copied back to R */
    if (T != R) {
        set_copy(R, T);
        set_delete(&T);
    }
}



void set_symmetric_difference(Set R, const Set A, const Set B) {
    Set A_minus_B, B_minus_A;

    set_create(&A_minus_B, A->equality);
    set_create(&B_minus_A, A->equality);

    set_difference(A_minus_B, A, B);
    set_difference(B_minus_A, B, A);
    set_union(R, A_minus_B, B_minus_A);

    set_delete(&A_minus_B);
    set_delete(&B_minus_A);
}



void set_apply_lifting_operator(Set R, const Set S, const SetLiftingOperator f, void *data) {
    unsigned int i;

    if (S == NULL) {
        set_clear(R);
        return;
    }

    R->capacity = S->capacity;
    R->size = S->size;
    R->elements = (void **) realloc(R->elements, R->capacity * sizeof(void *));
    R->equality = S->equality;

    for (i = 0; i < S->size; ++i) {
        const void *x = S->elements[i];
        R->elements[i] = f(x, data);
    }
}



void set_print(const Set S, const SetPrinter printer, FILE *stream) {
    unsigned int i;

    if (S == NULL) {
        fprintf(stream, "Set pointing to NULL.");
        return;
    }

    fprintf(stream, "Set @%p of size %u: {", (void *) S, S->size);
    for (i = 0; i < S->size; ++i) {
        if (printer != NULL) {
            printer(S->elements[i], stream);
        }
        else {
            fprintf(stream, "%p", S->elements[i]);
        }
        if (i + 1 < S->size) { 
            fprintf(stream, ", ");
        }
    }
    fprintf(stream, "}\n");
}



unsigned int set_equality_string(const void *x, const void *y) {
    return strcmp((const char *) x, (const char *) y) == 0;
}



void set_printer_string(const void *s, FILE *stream) {
    fprintf(stream, "%s", (const char *) s);
}
