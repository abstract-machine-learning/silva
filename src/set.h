/**
 * Defines a generic set.
 *
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 * @file set.h
 */
#ifndef SET_H
#define SET_H

#include <stdio.h>

/** Type of a set. */
typedef struct set *Set;


/** Type of an element eqaulity function. */
typedef unsigned int (*SetElementEquality)(const void *, const void *);


/** Type of a lifting operator. */
typedef void *(*SetLiftingOperator)(const void *, void *);


/** Type of a set property. */
typedef unsigned int (*SetProperty)(const void *, void *);


/** Type of a set printer. */
typedef void (*SetPrinter)(const void *, FILE *);


/**
 * Creates an empty set.
 *
 * @param[out] S Pointer to set to create
 * @param[in] equality Equality test for elements, uses pointer equality
 *            if set to NULL
 * @warning #set_delete should be called to ensure proper memory deallocation
 */
void set_create(Set *S, SetElementEquality equality);


/**
 * Deletes a set.
 *
 * @param[out] S Pointer to set to delete
 */
void set_delete(Set *S);


/**
 * Tells whether a set is empty: \f$S = \emptyset\f$.
 *
 * @param[in] S Set to test
 * @return 1 if set is empty, 0 otherwise
 */
unsigned int set_is_empty(const Set S);


/**
 * Tells whether a set is singleton: \f$ |S| = 1\f$.
 *
 * @param[in] S Set to test
 * @return 1 if set is a singleton, 0 otherwise
 */
unsigned int set_is_singleton(const Set S);


/**
 * Tells whether an element belongs to a set: \f$x \in S\f$.
 *
 * @param[in] S Set to test
 * @param[in] x Element to test
 * @return 1 if element belongs to given set, 0 otherwise
 */
unsigned int set_has_element(const Set S, const void *x);


/**
 * Tells whether a set is a subset of another set: \f$A \subseteq B\f$.
 *
 * @param[in] A First set
 * @param[in] B Second set
 * @return 1 if A is a subset of B, 0 otherwise
 */
unsigned int set_is_subset(const Set A, const Set B);


/**
 * Tells whether a set is a proper subset of another set: \f$A \subsetneq B\f$.
 *
 * @param[in] A First set
 * @param[in] B Second set
 * @return 1 if A is a proper subset of B, 0 otherwise
 */
unsigned int set_is_proper_subset(const Set A, const Set B);


/**
 * Tells whether a set is a superset of another set: \f$A \supseteq B\f$.
 *
 * @param[in] A First set
 * @param[in] B Second set
 * @return 1 if A is a superset of B, 0 otherwise
 */
unsigned int set_is_superset(const Set A, const Set B);


/**
 * Tells whether two sets are equal: \f$A = B\f$.
 *
 * @param[in] A First set
 * @param[in] B Second set
 * @return 1 if A is equal to B, 0 otherwise
 */
unsigned int set_is_equal(const Set A, const Set B);


/**
 * Tells whether two sets are disjoints: \f$A \cap B = \emptyset\f$.
 *
 * @param[in] A First set
 * @param[in] B Second set
 * @return 1 if A and B are disjoint
 */
unsigned int set_is_disjoint(const Set A, const Set B);


/**
 * Tells whether a property holds for every element in a set: \f$\forall x \in S\colon P(x)\f$.
 *
 * @param[in] S Set
 * @param[in] P Property
 * @param[in,out] data Additional data to be passed to P
 * @return 1 if property holds for every element, 0 otherwise
 */
unsigned int set_forall(const Set S, const SetProperty P, void *data);


/**
 * Tells whether a property holds for at least one element in a set: \f$\exists x \in S\colon P(x)\f$.
 *
 * @param[in] S Set
 * @param[in] P Property
 * @param[in,out] data Additional data to be passed to P
 * @return 1 if property holds for at least one element, 0 otherwise
 */
unsigned int set_exists(const Set S, const SetProperty P, void *data);



/**
 * Returns cardinality of a set: \f$|S|\f$.
 *
 * @param[in] S Set
 * @return Number of elements in given set
 */
unsigned int set_get_cardinality(const Set S);


/**
 * Returns elements in a set as an array.
 *
 * @param[in] S Set
 * @return Array containing elements of the set
 */
void *set_get_elements_as_array(const Set S);


/**
 * Returns the subset of a set which satisfyies a property: \f$R = \{x \in S | P(x)\}\f$.
 *
 * @param[out] R Resulting subset
 * @param[in] S Set
 * @param[in] P Property
 * @param[in,out] data Additional data to be passed to P
 */
void set_get_subset(Set R, const Set S, const SetProperty P, void *data);



/**
 * Copies a set into another set.
 *
 * @param[out] R Resulting set
 * @param[in] S Set
 */
void set_copy(Set R, const Set S);


/**
 * Removes every element in a set.
 *
 * @param[out] S Set
 */
void set_clear(Set S);


/**
 * Adds an element to a set: \f$S = S \cup \{x\}\f$.
 *
 * @param[in,out] S Set
 * @param[in] x Element
 * @note If given element already belongs to the set, nothing happens.
 */
void set_add_element(Set S, const void *x);


/**
 * Removes an element from a set: \f$S = S \setminus \{x\}\f$.
 *
 * @param[in,out] S Set
 * @param[in] x Element
 * @note If given element does not belongs to set nothing happens.
 */
void set_remove_element(Set S, const void *x);


/**
 * Computes set intersection: \f$R = A \cap B\f$.
 *
 * @param[out] R Resulting set
 * @param[in] A First set
 * @param[in] B Second set
 */
void set_intersection(Set R, const Set A, const Set B);


/**
 * Computes set union: \f$R = A \cup B\f$.
 *
 * @param[out] R Resulting set
 * @param[in] A First set
 * @param[in] B Second set
 */
void set_union(Set R, const Set A, const Set B);


/**
 * Computes set difference: \f$R = A \setminus B\f$.
 *
 * @param[out] R Resulting set
 * @param[in] A First set
 * @param[in] B Second set
 */
void set_difference(Set R, const Set A, const Set B);


/**
 * Computes set symmetric difference: \f$R = (A \setminus B) \cup (B \setminus A)\f$.
 *
 * @param[out] R Resulting set
 * @param[in] A First set
 * @param[in] B Second set
 */
void set_symmetric_difference(Set R, const Set A, const Set B);


/**
 * Applies a lifting operator over a set: \f$R = \{f(x) | x \in S\}\f$.
 *
 * @param[out] R Resulting set
 * @param[in] S Set
 * @param[in] f ifting operator
 * @param[in,out] data Additional data to be passed to the lifting operator
 */
void set_apply_lifting_operator(Set R, const Set S, const SetLiftingOperator f, void *data);


/**
 * Prints a set.
 *
 * @param[in] S Set
 * @param[in] printer Set printer, prints pointer address if set to NULL
 * @param[out] stream Stream
 */
void set_print(const Set S, const SetPrinter printer, FILE *stream);


/**
 * Equality test for strings.
 *
 * @param[in] x First string
 * @param[in] y Second string
 * @return 1 if and only if first string is equal to secon string
 */
unsigned int set_equality_string(const void *x, const void *y);


/**
 * Printer for strings.
 *
 * @param[in] s String
 * @param[in] stream Stream
 */
void set_printer_string(const void *s, FILE *stream);

#endif
