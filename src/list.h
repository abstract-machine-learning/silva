/**
 * Defines a generic list.
 *
 * @file list.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef LIST_H
#define LIST_H

#include <stdio.h>

/** Type of a list. */
typedef struct list *List;


/** Type of a list iterator. */
typedef void **ListIterator;


/** Type of a list printer. */
typedef void (*ListPrinter)(const void *, void *, FILE *);


/**
 * Creates an emtpy list.
 *
 * @param[out] L Pointer to list to create
 * @warning #list_delete should be called to ensure proper memory
 *          deallocation.
 */
void list_create(List *L);


/**
 * Deletes a list.
 *
 * @param[out] L Pointer to list to delete
 */
void list_delete(List *L);


/**
 * Copies content of an array into a list.
 *
 * @param[out] L List
 * @param[in] array Array
 * @param[in] n_elements Number of elements in array
 */
void list_from_array(List L, const void **array, const unsigned int n_elements);


/**
 * Wraps an array inside a list.
 *
 * @param[out] L List
 * @param[in] array Array
 * @param[in] n_elements Number of elements in array
 * @warning Content of array will be lost once #list_delete is called
 */
void list_wrap_array(List L, void ** const array, const unsigned int n_elements);



/**
 * Tells whether a list is empty.
 *
 * @param[in] L List
 * @return 1 if list contains no elements, 0 otherwise
 */
unsigned int list_is_empty(const List L);



/**
 * Returns number of elements of a list.
 *
 * @param[in] L List
 * @return Number of elements in the list
 */
unsigned int list_get_size(const List L);


/**
 * Returns element at given position.
 *
 * @param[in] L List
 * @param[in] i Position
 * @return Element at given position
 */
void *list_get_at(const List L, const unsigned int i);


/**
 * Returns first element in a list.
 *
 * @param[in] L List
 * @return First element
 */
void *list_get_head(const List L);


/**
 * Returns last element in a list.
 *
 * @param[in] L List
 * @return Last element
 */
void *list_get_last(const List L);


/**
 * Copies a list into an array.
 *
 * @param[out] array Array
 * @param[in] L List
 */
void list_to_array(void **array, const List L);


/**
 * Returns a list as an array.
 *
 * @param[in] L List
 * @return List as an array
 * @warning Any modification on the array (including deallocation) will
 *          have effect on the list.
 */
void **list_as_array(const List L);



/**
 * Adds an element at the end of a list.
 *
 * @param[in,out] L List
 * @param[in] x Element to add
 */
void list_append(List L, void * const x);


/**
 * Adds an element at the beginning of a list.
 *
 * @param[in,out] L List
 * @param[in] x Element to add
 */
void list_prepend(List L, void * const x);


/**
 * Adds an element at the end of a list.
 *
 * @param[in,out] L List
 * @param[in] x Element to add
 * @note This is an alias for #list_append.
 */
void list_push(List L, void * const x);


/**
 * Adds an element at the beginning of a list.
 *
 * @param[in,out] L List
 * @param[in] x Element to add
 * @note This is an alias for #list_prepend.
 */
void list_unshift(List L, void * const x);


/**
 * Removes and returns first element of a list.
 *
 * @param[in,out] L List
 * @return First element of the list
 */
void *list_shift(List L);


/**
 * Removes and return last element of a list.
 *
 * @param[in,out] L List
 * @return Last element of the list
 */
void *list_pop(List L);



/**
 * Prints a list.
 *
 * @param[in] L List
 * @param[in] printer List printer
 * @param[in,out] data Additional data to be passed to printer
 * @param[out] stream Stream
 */
void list_print(const List L, const ListPrinter printer, void * const data, FILE * const stream);



/***********************************************************************
 * List iterator functions.
 **********************************************************************/

/**
 * Returns iterator pointing to first element of a list.
 *
 * @param[in] L List
 * @return Iterator to first element
 */
ListIterator list_begin(const List L);


/**
 * Returns iterator pointing to last element of a list.
 *
 * @param[in] L List
 * @return Iterator to last element
 */
ListIterator list_last(const List L);


/**
 * Returns iterator pointing past the end of a list.
 *
 * @param[in] L List
 * @return Iterator pointing past the end
 */
ListIterator list_end(const List L);

#endif
