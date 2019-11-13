/**
 * Dataset.
 * 
 * Defines some utility function to manipulate a dataset.
 * 
 * @file dataset.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef DATASET_H
#define DATASET_H

#include <stdio.h>

/** Type of a dataset. */
typedef struct dataset *Dataset;


/** Types of dataset formats. */
typedef enum {
    DATASET_CSV,    /**< CSV dataset: \f$ \langle y, x_1, x_2, \ldots, x_n \rangle \f$. */
    DATASET_BINARY  /**< Binary format: \f$ \langle y, x_1, x_2, \ldots, x_n \rangle \f$. */
} DatasetFormat;



/**
 * Reads a dataset from a source.
 *
 * Recognizes automatically the format of the file.
 * 
 * @param[in,out] stream Source to read from
 * @return Dataset read from source
 */
Dataset dataset_read(FILE *stream);


/**
 * Writes a dataset to a destination.
 *
 * @param[in] dataset Dataset to write
 * @param[in] format Destination dataset format
 * @param[out] stream Stream
 */
void dataset_write(
    const Dataset dataset,
    const DatasetFormat format,
    FILE *stream
);


/**
 * Deletes a dataset.
 * 
 * @param[out] dataset Pointer to dataset to delete
 */
void dataset_delete(Dataset *dataset);


/**
 * Returns number of entries in given dataset.
 * 
 * @param[in] dataset Dataset
 * @return Number of entries
 */
unsigned int dataset_get_size(const Dataset dataset);


/**
 * Returns number of features in given dataset.
 * 
 * @param[in] dataset Dataset
 * @return Number of features
 */
unsigned int dataset_get_space_size(const Dataset dataset);


/**
 * Returns data in i-esim entry of given dataset.
 * 
 * @param[in] dataset Dataset
 * @param[in] i       Index of entry to read
 * @return Pointer to data of i-esim entry
 */
double *dataset_get_row(const Dataset dataset, const unsigned int i);


/**
 * Returns label of i-esim entry of given dataset.
 * 
 * @param[in] dataset Dataset
 * @param[in] i       Index of entry to read
 * @return Label of i-esim entry
 */
char *dataset_get_label(const Dataset dataset, const unsigned int i);

#endif
