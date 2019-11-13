/**
 * Implements a perturbation.
 * 
 * @file perturbation.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "perturbation.h"

/***********************************************************************
 * Internal functinos.
 **********************************************************************/

/**
 * Prints an \f$L_\infty\f$ perturbation.
 *
 * @param[in] perturbation Perturbation
 * @param[out] stream Stream
 */
static void perturbation_print_l_inf(
    const Perturbation perturbation,
    FILE *stream
) {
    fprintf(stream, "L_inf_%g", perturbation.data.l_inf.magnitude);
}



/**
 * Prints a clipped \f$L_\infty\f$ perturbation.
 *
 * @param[in] perturbation Perturbation
 * @param[out] stream Stream
 */
static void perturbation_print_l_inf_clip_all(
    const Perturbation perturbation,
    FILE *stream
) {
    fprintf(
        stream,
        "L_inf_%g in [%g; %g]",
        perturbation.data.l_inf_clip_all.magnitude,
        perturbation.data.l_inf_clip_all.min,
        perturbation.data.l_inf_clip_all.max
    );
}



/***********************************************************************
 * Public functions.
 **********************************************************************/

void perturbation_print(const Perturbation perturbation, FILE *stream) {
    switch (perturbation.type) {
        case PERTURBATION_L_INF:
            perturbation_print_l_inf(perturbation, stream);
            break;

        case PERTURBATION_L_INF_CLIP_ALL:
            perturbation_print_l_inf_clip_all(perturbation, stream);
            break;
    }
}
