/**
 * Data mapper for a classifier in silva format.
 *
 * @file classifier_silva.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef CLASSIFIER_SILVA_H
#define CLASSIFIER_SILVA_H

#include <stdio.h>

#include "../classifier.h"


/**
 * Reads a classifier stored in silva format.
 *
 * @param[out] C Pointer to classifier to read
 * @param[in,out] stream Stream
 * @warning #classifier_delete should be called to ensure proper memory
 *          deallocation.
 */
void classifier_silva_read(Classifier *C, FILE *stream);

#endif
