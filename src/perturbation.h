/**
 * Defines a perturbation.
 * 
 * @file perturbation.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef PERTURBATION_H
#define PERTURBATION_H

#include <stdio.h>

/** Types of perturbation. */
typedef enum {
    PERTURBATION_L_INF,           /**< \f$L_\infty\f$ perturbation. */
    PERTURBATION_L_INF_CLIP_ALL,  /**< As \f$L_\infty\f$, but every dimension is bound. */
    PERTURBATION_FROM_FILE        /**< Reads hyperrectangle adversarial regions from file. */
} PerturbationType;


/** Structure of l_inf perturbation data. */
struct l_inf_data {
    double magnitude;  /**< Radius of l_inf perturbation. */
};


/** Structure of l_inf clip all perturbation data. */
struct l_inf_clip_all_data {
    double magnitude;  /**< Radius of l_inf perturbation. */
    double min;        /**< Minimum allowed value. */
    double max;        /**< Maximum allowed value. */
};


/** Structure of a perturbation from file. */
struct from_file_data {
    FILE *stream;  /**< Pointer to stream. */
};


/** Union of perturbation data. */
union perturbation_data {
    struct l_inf_data l_inf;                    /**< Data of l_inf perturbation. */
    struct l_inf_clip_all_data l_inf_clip_all;  /**< Data of clipped l_inf perturbation. */
    struct from_file_data from_file;            /**< Data of file perturbation. */
};


/** Structure of a perturbation. */
struct perturbation {
    PerturbationType type;  /**< Type of perturbation. */
    union perturbation_data data;  /**< Perturbation-specific data. */
};


/** Type of a perturbation. */
typedef struct perturbation Perturbation;



/**
 * Prints a perturbation.
 *
 * @param[in] perturbation Perturbation
 * @param[out] stream Stream
 */
void perturbation_print(const Perturbation perturbation, FILE *stream);

#endif
