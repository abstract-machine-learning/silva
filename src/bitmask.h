/**
 * Defines a bitmask.
 *
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 * @file bitmask.h
 */
#ifndef BITMASK_H
#define BITMASK_H

#include <stdio.h>

/** Type of a bitmask. */
typedef struct bitmask *Bitmask;


/** Type of a bitmask property. */
typedef unsigned int (*BitmaskProperty)(const unsigned int, void *);



/**
 * Creates an empty bitmask.
 *
 * @param[out] B Pointer to bitmask to create
 * @warning #bitmask_delete should be called to ensure proper memory
 *          deallocation
 */
void bitmask_create(Bitmask *B);


/**
 * Deletes a bitmask.
 *
 * @param[out] B Pointer to bitmask to delete
 */
void bitmask_delete(Bitmask *B);



/**
 * Tells whether a bitmask is empty: \f$B = \emptyset\f$.
 *
 * @param[in] B Bitmask
 * @return 1 if bitmask has no active bits, 0 otherwise
 */
unsigned int bitmask_is_empty(const Bitmask B);


/**
 * Tells whether a bitmask has a single bit set: \f$|B| = 1\f$.
 *
 * @param[in] B Bitmask
 * @return 1 if bitmask has a signle bit set, 0 otherwise
 */
unsigned int bitmask_is_singleton(const Bitmask B);


/**
 * Tells whether given bit is active: \f$x \in B\f$.
 *
 * @param[in] B Bitmask
 * @param[in] x Bit
 * @return 1 if given bit is active, 0 otherwise
 */
unsigned int bitmask_has_element(const Bitmask B, const unsigned int x);


/**
 * Tells whether a bitmask is a subset of another bitmask: \f$A \sqsubseteq B\f$.
 *
 * @param[in] A First bitmask
 * @param[in] B Second bitmask
 * @return 1 if first bitmask is a subset of the second one, 0 otherwise
 */
unsigned int bitmask_is_subset(const Bitmask A, const Bitmask B);


/**
 * Tells whether a bitmask is a proper subset of another bitmask:
 * \f$A \sqsubseteq B \wedge A \neq B\f$.
 *
 * @param[in] A First bitmask
 * @param[in] B Second bitmask
 * @return 1 if first bitmask is a proper subset of the second one, 0 otherwise
 */
unsigned int bitmask_is_proper_subset(const Bitmask A, const Bitmask B);


/**
 * Tells whether a bitmask is a superset of another bitmask: \f$A \sqsupseteq B\f$.
 *
 * @param[in] A First bitmask
 * @param[in] B Second bitmask
 * @return 1 if first bitmask is a superset of the second one, 0 otherwise
 */
unsigned int bitmask_is_superset(const Bitmask A, const Bitmask B);


/**
 * Tells whether two bitmask are equal: \f$A = B\f$.
 *
 * @param[in] A First bitmask
 * @param[in] B Second bitmask
 * @return 1 if A is equal to B, 0 otherwise
 */
unsigned int bitmask_is_equal(const Bitmask A, const Bitmask B);


/**
 * Tells whether two bitmasks are disjoint: \f$A \sqcap B = \emptyset\f$.
 *
 * @param[in] A First bitmask
 * @param[in] B Second bitmask
 * @return 1 if A and B are disjoint, 0 otherwise
 */
unsigned int bitmask_is_disjoint(const Bitmask A, const Bitmask B);


/**
 * Tells wheter a property holds for every active bit of a bitmask:
 * \f$\forall x \in B\colon P(x)\f$.
 *
 * @param[in] B Bitmask
 * @param[in] P Property
 * @param[in,out] data Additional data to be passed to P
 * @return 1 if property holds for every active bit, 0 otherwise
 */
unsigned int bitmask_forall(const Bitmask B, const BitmaskProperty P, void *data);


/**
 * Tells wheter a property holds for at least one active bit of a bitmask:
 * \f$\exists x \in B\colon P(x)\f$.
 *
 * @param[in] B Bitmask
 * @param[in] P Property
 * @param[in,out] data Additional data to be passed to P
 * @return 1 if property holds for at least one active bit, 0 otherwise
 */
unsigned int bitmask_exists(const Bitmask B, const BitmaskProperty P, void *data);



/**
 * Returns number of active bits in a bitmask: \f$|B|\f$.
 *
 * Uses Brian Kernighan's algorithm.
 *
 * @param[in] B Bitmask
 * @return Number of active bits in the bitmask
 */
unsigned int bitmask_get_cardinality(const Bitmask B);



/**
 * Copies a bitmask.
 *
 * @param[out] R Destination bitmask
 * @param[in] B Source bitmask
 */
void bitmask_copy(Bitmask R, const Bitmask B);


/**
 * Deactivates every bit in a bitmask: \f$B = \emptyset\f$.
 *
 * @param[out] B Bitmask
 */
void bitmask_clear(Bitmask B);


/**
 * Activates a bit in a bitmask: \f$B = B \sqcup \{x\}\f$.
 *
 * @param[in,out] B Bitmask
 * @param[in] x Bit to activate
 */
void bitmask_add_element(Bitmask B, const unsigned int x);


/**
 * Deactivates a bit in a bitmask:  \f$B = B \setminus \{x\}\f$.
 *
 * @param[in,out] B Bitmask
 * @param[in] x Bit to deactivate
 */
void bitmask_remove_element(Bitmask B, const unsigned int x);


/**
 * Computes conjunction of active bits: \f$R = A \sqcap B\f$.
 *
 * @param[out] R Resulting bitmask
 * @param[in] A First bitmask
 * @param[in] B Second bitmask
 */
void bitmask_intersection(Bitmask R, const Bitmask A, const Bitmask B);


/**
 * Computes disjunction of active bits: \f$R = A \sqcup B\f$.
 *
 * @param[out] R Resulting bitmask
 * @param[in] A First bitmask
 * @param[in] B Second bitmask
 */
void bitmask_union(Bitmask R, const Bitmask A, const Bitmask B);


/**
 * Computes bits which are active in first mask and not active in the
 * second: \f$R = A \setminus B\f$.
 *
 * @param[out] R Resulting bitmask
 * @param[in] A First bitmask
 * @param[in] B Second bitmask
 */
void bitmask_difference(Bitmask R, const Bitmask A, const Bitmask B);


/**
 * Computes disjunction of bits which are active in first mask and not active in
 * the second and vice-versa: \f$R = (A \setminus B) \sqcup (B \setminus A)\f$.
 *
 * @param[out] R Resulting bitmask
 * @param[in] A First bitmask
 * @param[in] B Second bitmask
 */
void bitmask_symmetric_difference(Bitmask R, const Bitmask A, const Bitmask B);


/**
 * Prints a bitmask.
 *
 * @param[in] B Bitmask
 * @param[out] stream Stream
 */
void bitmask_print(const Bitmask B, FILE *stream);

#endif
