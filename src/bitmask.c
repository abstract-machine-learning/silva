/**
 * Implements a bitmask.
 *
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 * @file bitmask.c
 */
#include "bitmask.h"

#include <stdlib.h>


/** Maximum number of bits in a bitmask, architecture-dependent. */
#define BITMASK_CAPACITY (sizeof(unsigned long long int) << 3)


/** Structure of a bitmask. */
struct bitmask {
    unsigned long long int bits;  /**< Array of bits. */
};



void bitmask_create(Bitmask *B) {
    Bitmask b = (Bitmask) malloc(sizeof(struct bitmask));
    if (b == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    b->bits = 0;

    *B = b;
}



void bitmask_delete(Bitmask *B) {
    if (B == NULL || *B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    free(*B);
    *B = NULL;
}



unsigned int bitmask_is_empty(const Bitmask B) {
    if (B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return B->bits == 0;
}



unsigned int bitmask_is_singleton(const Bitmask B) {
    if (B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return (B->bits != 0) && (B->bits & (B->bits - 1)) == 0;
}



unsigned int bitmask_has_element(const Bitmask B, const unsigned int x) {
    if (B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return x < BITMASK_CAPACITY ? B->bits >> x & 0x1 : 0;
}



unsigned int bitmask_is_subset(const Bitmask A, const Bitmask B) {
    if (A == NULL || B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return (A->bits & B->bits) == A->bits;
}



unsigned int bitmask_is_proper_subset(const Bitmask A, const Bitmask B) {
    return bitmask_is_subset(A, B) && A->bits != B->bits;
}



unsigned int bitmask_is_superset(const Bitmask A, const Bitmask B) {
    return bitmask_is_subset(B, A);
}



unsigned int bitmask_is_equal(const Bitmask A, const Bitmask B) {
    if (A == NULL || B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return A->bits == B->bits;
}



unsigned int bitmask_is_disjoint(const Bitmask A, const Bitmask B) {
    if (A == NULL || B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return (A->bits & B->bits) == 0;
}



unsigned int bitmask_forall(const Bitmask B, const BitmaskProperty P, void *data) {
    unsigned int i;

    if (B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    for (i = 0; i < BITMASK_CAPACITY; ++i) {
        if (B->bits >> i & 0x1 && !P(i, data)) {
            return 0;
        }
    }

    return 1;
}



unsigned int bitmask_exists(const Bitmask B, const BitmaskProperty P, void *data) {
    unsigned int i;

    if (B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    for (i = 0; i < BITMASK_CAPACITY; ++i) {
        if (B->bits >> i & 0x1 && P(i, data)) {
            return 1;
        }
    }

    return 0;
}



unsigned int bitmask_get_cardinality(const Bitmask B) {
    unsigned int n_elements = 0;
    unsigned long long int bits;

    if (B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    bits = B->bits;
    while (bits) {
        bits &= bits - 1;
        ++n_elements;
    }

    return n_elements;
}



void bitmask_copy(Bitmask R, const Bitmask B) {
    if (R == NULL || B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    R->bits = B->bits;
}



void bitmask_clear(Bitmask B) {
    if (B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    B->bits = 0;
}



void bitmask_add_element(Bitmask B, const unsigned int x) {
    if (B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    if (x >= BITMASK_CAPACITY) {
        fprintf(stderr, "[%s: %d] Trying to set bit %u on a bitmask with capacity of %lu.\n", __FILE__, __LINE__, x, BITMASK_CAPACITY);
    }

    B->bits |= 0x1 << x;
}



void bitmask_remove_element(Bitmask B, const unsigned int x) {
    if (B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    if (x >= BITMASK_CAPACITY) {
        fprintf(stderr, "[%s: %d] Trying to unset bit %u on a bitmask with capacity of %lu.\n", __FILE__, __LINE__, x, BITMASK_CAPACITY);
        abort();
    }

    B->bits &= ~(0x1 << x);
}



void bitmask_intersection(Bitmask R, const Bitmask A, const Bitmask B) {
    if (R == NULL || A == NULL || B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    R->bits = A->bits & B->bits;
}



void bitmask_union(Bitmask R, const Bitmask A, const Bitmask B) {
    if (R == NULL || A == NULL || B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    R->bits = A->bits | B->bits;
}



void bitmask_difference(Bitmask R, const Bitmask A, const Bitmask B) {
    if (R == NULL || A == NULL || B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    R->bits = A->bits & ~B->bits;
}



void bitmask_symmetric_difference(Bitmask R, const Bitmask A, const Bitmask B) {
    if (R == NULL || A == NULL || B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    R->bits = (A->bits & ~B->bits) | (B->bits & ~A->bits);
}



void bitmask_print(const Bitmask B, FILE *stream) {
    unsigned int n_elements;
    unsigned int i, counter = 0;

    if (B == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    n_elements = bitmask_get_cardinality(B);
    fprintf(stream, "Bitmask @%p, with %u elements: {", (void *) B, n_elements);
    for (i = 0; i < BITMASK_CAPACITY; ++i) {
        if (bitmask_has_element(B, i)) {
            fprintf(stream, "%u", i);
            ++counter;
            if (counter < n_elements) {
                fprintf(stream, ", ");
            }
        }
    }
    fprintf(stream, "}");
}
