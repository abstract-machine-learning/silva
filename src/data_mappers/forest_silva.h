/**
 * Data mapper for a random forest to file system.
 *
 * @file forest_silva.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef FOREST_SILVA_H
#define FOREST_SILVA_H

#include <stdio.h>

#include "../forest.h"


/**
 * Reads a forest stored in silva format.
 *
 * @param[out] F Pointer to forest
 * @param[in,out] stream Stream
 * @warning #forest_delete should be called to ensure proper memory
 *          deallocation.
 */
void forest_silva_read(Forest *F, FILE *stream);

#endif
