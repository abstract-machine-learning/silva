/**
 * Hyperrectangle abstract domain \f$\mathcal{Hyp}\f$.
 *
 * Hyperrectangles are generalization of intervals in spaces of arbitrary
 * (finite) dimension \f$\mathbb{R}^n\f$.
 *
 * @file hyperrectangle.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef HYPERRECTANGLE_H
#define HYPERRECTANGLE_H

#include "interval.h"

#include <stdlib.h>
#include <string.h>


/** Type of a hyperrectangle. */
typedef struct hyperrectangle *Hyperrectangle;


/** Structure of a hyperrectangle. */
struct hyperrectangle {
    Interval *intervals;  /**< Orthogonal intervals. */
    unsigned int n;       /**< Size of the space. */
};



/**
 * Creates a hyperrectangle.
 *
 * @param[out] H Hyperrectangle
 * @param[in] n Space size
 * @warning #hyperrectangle_delete should be called to ensure proper memory
 *          deallocation.
 */
static inline void hyperrectangle_create(Hyperrectangle *H, const unsigned int n) {
    Hyperrectangle h = (Hyperrectangle) malloc(sizeof(struct hyperrectangle));
    h->intervals = (Interval *) malloc(n * sizeof(Interval));
    h->n = n;

    *H = h;
}



/**
 * Copies a hyperrectangle.
 *
 * @param[out] r Result
 * @param[in] x Hyperrectangle to copy
 */
static inline void hyperrectangle_copy(Hyperrectangle r, const Hyperrectangle x) {
    if (r->n != x->n) {
        r->intervals = (Interval *) realloc(r->intervals, x->n * sizeof(Interval));
    }

    r->n = x->n;
    memcpy(r->intervals, x->intervals, x->n * sizeof(Interval));
}



/**
 * Deletes a hyperrectangle.
 *
 * @param[out] H Hyperrectangle
 */
static inline void hyperrectangle_delete(Hyperrectangle *H) {
    free((*H)->intervals);
    free(*H);
    *H = NULL;
}



/**
 * Tells whether a hyperrectangle is bottom: \f$x = \bot\f$.
 *
 * @param[in] x Hyperrectangle
 * @return 1 if hyperrectangle is bottom, 0 otherwise
 */
static inline unsigned int hyperrectangle_is_bottom(const Hyperrectangle x) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        if (interval_is_bottom(x->intervals[i])) {
            return 1;
        }
    }

    return 0;
}



/**
 * Returns size of space of a hyperrectangle.
 *
 * @param[in] x Hyperrectangle
 * @return Size of space
 */
static inline unsigned int hyperrectangle_get_space_size(const Hyperrectangle x) {
    return x->n;
}



/**
 * Returns center of a hyperrectangle.
 *
 * @param[out] c Center of hyperrectangle
 * @param[in] x Hyperrectangle
 */
static inline void hyperrectangle_midpoint(Real *c, const Hyperrectangle x) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        c[i] = interval_midpoint(x->intervals[i]);
    }
}



/**
 * Returns radius of a hyperrectangle.
 *
 * @param[out] r Radius of hyperrectangle
 * @param[in] x Hyperrectangle
 */
static inline void hyperrectangle_radius(Real *r, const Hyperrectangle x) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        r[i] = interval_radius(x->intervals[i]);
    }
}



/**
 * Returns a random point in a hyperrectangle.
 *
 * @param[out] sample Randomly chosen point in given hyperrectangle
 * @param[in] x Hyperrectangle
 */
static inline void hyperrectangle_sample(Real *sample, const Hyperrectangle x) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        sample[i] = interval_sample(x->intervals[i]);
    }
}



/**
 * Returns generalized volume of hyperrectangle.
 *
 * Computes volume as the product of the radius for each dimension:
 * \f$Volume(x) = \prod_{0 \leq i < n} radius(x)_i\f$.
 *
 * @param[in] x Hyperrectangle
 * @return Volume of hyperrectangle
 */
static inline Real hyperrectangle_volume(const Hyperrectangle x) {
    unsigned int i;
    Real volume = 1.0;

    for (i = 0; i < x->n; ++i) {
        volume *= interval_radius(x->intervals[i]);
    }

    return volume;
}



/**
 * Computes \f$r = x +^{\mathcal{Hyp}} y\f$.
 *
 * @param[out] r Result
 * @param[in] x First addendum
 * @param[in] y Second addendum
 */
static inline void hyperrectangle_add(Hyperrectangle r, const Hyperrectangle x, const Hyperrectangle y) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        interval_add(r->intervals + i, x->intervals[i], y->intervals[i]);
    }
}



/**
 * Computes \f$r = x -^{\mathcal{Hyp}} y\f$.
 *
 * @param[out] r Result
 * @param[in] x Minuendum
 * @param[in] y Subtrahendum
 */
static inline void hyperrectangle_sub(Hyperrectangle r, const Hyperrectangle x, const Hyperrectangle y) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        interval_sub(r->intervals + i, x->intervals[i], y->intervals[i]);
    }
}



/**
 * Computes \f$r = x \cdot^\mathcal{Hyp} y\f$.
 *
 * @param[out] r Result
 * @param[in] x First factor
 * @param[in] y Second factor
 */
static inline void hyperrectangle_mul(Hyperrectangle r, const Hyperrectangle x, const Hyperrectangle y) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        interval_mul(r->intervals + i, x->intervals[i], y->intervals[i]);
    }
}



/**
 * Computes \f$r = x^{degree}\f$.
 *
 * @param[out] r Result
 * @param[in] x Base (hyperrectangle)
 * @param[in] degree Natural exponent
 */
static inline void hyperrectangle_pow(Hyperrectangle r, const Hyperrectangle x, const unsigned int degree) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        interval_pow(r->intervals + i, x->intervals[i], degree);
    }
}



/**
 * Computes \f$e^x\f$.
 *
 * @param[out] r Result
 * @param[in] x Exponent (hyperrectangle)
 */
static inline void hyperrectangle_exp(Hyperrectangle r, const Hyperrectangle x) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        interval_exp(r->intervals + i, x->intervals[i]);
    }
}



/**
 * Computes \f$r = x +^\mathcal{Hyp} t\f$.
 *
 * @param[out] r Result
 * @param[in] x Hyperrectangle
 * @param[in] t Translation vector
 */
static inline void hyperrectangle_translate(Hyperrectangle r, const Hyperrectangle x, const Real *t) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        interval_translate(r->intervals + i, x->intervals[i], t[i]);
    }
}



/**
 * Computes \f$r = diag(s) \cdot^\mathcal{Hyp} x\f$.
 *
 * @param[out] r Result
 * @param[in] x Hyperrectangle
 * @param[in] s Scaling factor
 */
static inline void hyperrectangle_scale(Hyperrectangle r, const Hyperrectangle x, const Real *s) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        interval_scale(r->intervals + i, x->intervals[i], s[i]);
    }
}



/**
 * Computes \f$r = s \cdot^\mathcal{Hyp} x\f$.
 *
 * @param[out] r Result
 * @param[in] x Hyperrectangle
 * @param[in] s Scaling factor
 */
static inline void hyperrectangle_scale_homogeneous(Hyperrectangle r, const Hyperrectangle x, const Real s) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        interval_scale(r->intervals + i, x->intervals[i], s);
    }
}



/**
 * Computes fused multiply-add \f$r = (\alpha \cdot^\mathcal{Hyp} x) +^\mathcal{Hyp} y\f$.
 *
 * @param[out] r Result
 * @param[in] alpha Scaling factor
 * @param[in] x First addendum
 * @param[in] y Second addendum
 */
static inline void hyperrectangle_fma(Hyperrectangle r, const Real alpha, const Hyperrectangle x, const Hyperrectangle y) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        interval_fma(r->intervals + i, alpha, x->intervals[i], y->intervals[i]);
    }
}



/**
 * Computes greatest lowerbound \f$r = x \sqcap^\mathcal{Hyp} y\f$.
 *
 * @param[out] r Result
 * @param[in] x First interval
 * @param[in] y Second interval
 */
static inline void hyperrectangle_glb(Hyperrectangle r, const Hyperrectangle x, const Hyperrectangle y) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        interval_glb(r->intervals + i, x->intervals[i], y->intervals[i]);
        if (interval_is_bottom(r->intervals[i])) {
            r->intervals[0].l = +1.0;
            r->intervals[0].u = -1.0;
        }
    }
}



/**
 * Computes least upperbound \f$r = x \sqcup^\mathcal{Hyp} y\f$.
 *
 * @param[out] r Result
 * @param[in] x First interval
 * @param[in] y Second interval
 */
static inline void hyperrectangle_lub(Hyperrectangle r, const Hyperrectangle x, const Hyperrectangle y) {
    unsigned int i;

    for (i = 0; i < x->n; ++i) {
        interval_lub(r->intervals + i, x->intervals[i], y->intervals[i]);
    }
}



/**
 * Prints a hyperrectangle.
 *
 * @param[in] x Hyperrectangle
 * @param[out] stream Stream
 */
static inline void hyperrectangle_print(const Hyperrectangle x, FILE *stream) {
    unsigned int i;

    fprintf(stream, "Hyperrectangle @%p, in R^%u:\n", (void *) x, x->n);
    for (i = 0; i < x->n; ++i) {
        fprintf(stream, "\t");
        interval_print(x->intervals[i], stream);
        fprintf(stream, "\n");
    }
}

#endif
