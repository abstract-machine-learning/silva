/**
 * Interval abstract domain \f$\mathcal{Int}\f$.
 *
 * Defines abstract transfer functions for intervals. An interval is a
 * close subset of \f$\mathbb{R}\f$ which includes every point greater
 * than or equal to a lowerbound \f$l \in \mathbb{R}\f$ and less than
 * or equal to an upperbound \f$u \in \mathbb{R}\f$:
 * \f$\{x \in \mathbb{R} | l \leq x \leq u\}\f$.  An interval can be
 * represented symbolically as a pair
 * \f$\langle l, u \rangle \in \mathbb{R}^2\f$.
 *
 * @file interval.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef INTERVAL_H
#define INTERVAL_H

#include <stdlib.h>

#include "../type.h"

/** Type of an interval. */
typedef struct interval Interval;


/** Structure of an interval. */
struct interval {
    Real l;  /**< Lowerbound. */
    Real u;  /**< Upperbound. */
};



/**
 * Tells whether an interval is bottom: \f$x = \bot\f$.
 *
 * An interval \f$x = \langle l, u \rangle\f$ is considered bottom whenever
 * \f$l > u\f$.
 *
 * @param[in] x Interval
 * @return 1 if interval is bottom, 0 otherwise
 */
static inline unsigned int interval_is_bottom(const Interval x) {
    return x.l > x.u;
}


/**
 * Tells whether an interval is strictly dominated by another: \f$x \sqsubseteq^{Int} y\f$.
 *
 * @param[in] x First interval
 * @param[in] y Second interval
 * @return 1 if and only if first interval is strictly dominated by second interval
 */
static inline unsigned int interval_is_lt(const Interval x, const Interval y) {
    return x.u < y.l;
}


/**
 * Tells whether an interval is dominated by another: \f$x \sqsubseteq^{Int} y\f$.
 *
 * @param[in] x First interval
 * @param[in] y Second interval
 * @return 1 if and only if first interval is dominated second interval
 */
static inline unsigned int interval_is_leq(const Interval x, const Interval y) {
    return x.u <= y.l;
}



/**
 * Returns center of an interval.
 *
 * @param[in] x Interval
 * @return Center of given interval
 */
static inline Real interval_midpoint(const Interval x) {
    return (x.l + x.u) * 0.5;
}



/**
 * Returns radius of an interval.
 *
 * @param[in] x Interval
 * @return Radius of given interval
 */
static inline Real interval_radius(const Interval x) {
    return (x.u - x.l) * 0.5;
}



/**
 * Returns a random point in a interval.
 *
 * @param[in] x Interval
 * @return Randomly chosen point in given interval
 */
static inline Real interval_sample(const Interval x) {
    return (Real) rand() / RAND_MAX * (x.u - x.l) + x.l;
}



/**
 * Computes \f$r = x +^{\mathcal{Int}} y\f$.
 *
 * @param[out] r Result
 * @param[in] x First addendum
 * @param[in] y Second addendum
 */
static inline void interval_add(Interval *r, const Interval x, const Interval y) {
    round_down;
    r->l = x.l + y.l;
    round_up;
    r->u = x.u + y.u;
}



/**
 * Computes \f$r = x -^\mathcal{Int} y\f$.
 *
 * @param[out] r Result
 * @param[in] x Minuendum
 * @param[in] y Subtrahendum
 */
static inline void interval_sub(Interval *r, const Interval x, const Interval y) {
    round_down;
    r->l = x.l - y.l;
    round_up;
    r->u = x.u - y.u;
}



/**
 * Computes \f$r = x \cdot^\mathcal{Int} y\f$.
 *
 * @param[out] r Result
 * @param[in] x First factor
 * @param[in] y Second factor
 */
static inline void interval_mul(Interval *r, const Interval x, const Interval y) {
    if ((x.l == 0.0 && x.u == 0.0) || (y.l == 0.0 && y.u == 0.0)) {
        r->l = 0.0;
        r->u = 0.0;
        return;
    }

    if (x.l >= 0.0) {
        if (y.l >= 0.0) {
            round_down;
            r->l = x.l * y.l;
            round_up;
            r->u = x.u * y.u;
        }

        else if (y.u <= 0.0) {
            round_down;
            r->l = x.u * y.l;
            round_up;
            r->u = x.l * y.u;
        }

        else {
            round_down;
            r->l = x.u * y.l;
            round_up;
            r->u = x.u * y.u;
        }
    }

    else if (x.u <= 0.0) {
        if (y.l >= 0.0) {
            round_down;
            r->l = x.l * y.u;
            round_up;
            r->u = x.u * y.l;
        }
        else if (y.u <= 0.0) {
            round_down;
            r->l = x.u * y.u;
            round_up;
            r->u = x.l * y.l;
        }
        else {
            round_down;
            r->l = x.l * y.u;
            round_up;
            r->u = x.l * y.l;
        }
    }

    else {
        if (y.l >= 0.0) {
            round_down;
            r->l = x.l * y.u;
            round_up;
            r->u = x.u * y.u;
        }
        else if (y.u <= 0.0) {
            round_down;
            r->l = x.u * y.l;
            round_up;
            r->u = x.l * y.l;
        }
        else {
            round_down;
            r->l = min(x.l * y.u, x.u * y.l);
            round_up;
            r->u = max(x.l * y.l, x.u * y.u);
        }
    }
}



/**
 * Computes \f$r = x^{degree}\f$.
 *
 * @param[out] r Result
 * @param[in] x Base (interval)
 * @param[in] degree Natural exponent
 */
static inline void interval_pow(Interval *r, const Interval x, const unsigned int degree) {
    unsigned int i;

    *r = x;
    for (i = 1; i < degree; ++i) {
        interval_mul(r, *r, x);
    }
}



/**
 * Computes \f$r = e^x\f$.
 *
 * @param[out] r Result
 * @param[in] x Exponent (interval)
 */
static inline void interval_exp(Interval *r, const Interval x) {
    round_down;
    r->l = exp(x.l);
    round_up;
    r->u = exp(x.u);
}



/**
 * Computes \f$r = x +^\mathcal{Int} t\f$.
 *
 * @param[out] r Result
 * @param[in] x Interval
 * @param[in] t Translation length
 */
static inline void interval_translate(Interval *r, const Interval x, const Real t) {
    round_down;
    r->l = x.l + t;
    round_up;
    r->u = x.u + t;
}



/**
 * Computes \f$r = s \cdot^\mathcal{Int} x\f$.
 *
 * @param[out] r Result
 * @param[in] x Interval
 * @param[in] s Scaling factor
 */
static inline void interval_scale(Interval *r, const Interval x, const Real s) {
    if (s >= 0.0) {
        round_down;
        r->l = s * x.l;
        round_up;
        r->u = s * x.u;
    }
    else {
        round_down;
        r->l = s * x.u;
        round_up;
        r->u = s * x.l;
    }
}



/**
 * Coputes fused multiply-add \f$r = (\alpha \cdot^\mathcal{Int} x) +^\mathcal{Int} y\f$.
 *
 * @param[out] r Result
 * @param[in] alpha Scaling factor
 * @param[in] x First addendum
 * @param[in] y Second addendum
 */
static inline void interval_fma(Interval *r, const Real alpha, const Interval x, const Interval y) {
    if (alpha >= 0) {
        round_down;
        r->l = alpha * x.l + y.l;
        round_up;
        r->u = alpha * x.u + y.u;
    }
    else {
        round_down;
        r->l = alpha * x.u + y.l;
        round_up;
        r->u = alpha * x.l + y.u;
    }
}



/**
 * Computes greatest lowerbound \f$r = x \sqcap^\mathcal{Int} y\f$.
 *
 * @param[out] r Result
 * @param[in] x First interval
 * @param[in] y Second interval
 */
static inline void interval_glb(Interval *r, const Interval x, const Interval y) {
    r->l = x.l > y.l ? x.l : y.l;
    r->u = x.u < y.u ? x.u : y.u;
}



/**
 * Computes least upperbound \f$r = x \sqcup^\mathcal{Int} y\f$.
 *
 * @param[out] r Result
 * @param[in] x First interval
 * @param[in] y Second interval
 */
static inline void interval_lub(Interval *r, const Interval x, const Interval y) {
    r->l = x.l < y.l ? x.l : y.l;
    r->u = x.u > y.u ? x.u : y.u;
}



/**
 * Prints an interval.
 *
 * @param[in] x Interval
 * @param[out] stream Stream
 */
static inline void interval_print(const Interval x, FILE *stream) {
    if (x.l <= x.u) {
        fprintf(stream, "[%g; %g]", x.l, x.u);
    }
    else {
        fprintf(stream, "bottom");
    }
}



/**
 * Prints an interval in a compact form.
 *
 * @param[in] x Interval
 * @param[out] stream Stream
 */
static inline void interval_dump(const Interval x, FILE *stream) {
    if (x.l <= x.u) {
        fprintf(stream, "[%g,%g]", x.l, x.u);
    }
    else {
        fprintf(stream, "bottom");
    }
}

#endif
