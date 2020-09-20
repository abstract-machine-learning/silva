/**
 * Hyperrectangle analysis for a classifier.
 *
 * @file classifier_hyperrectangle.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "classifier_hyperrectangle.h"

#include "decision_tree_hyperrectangle.h"
#include "forest_hyperrectangle.h"


/***********************************************************************
 * Internal functions.
 **********************************************************************/

/**
 * Converts an \f$L_\infty\f$ #AdversarialRegion into a #Hyperrectangle.
 * 
 * @param[out] x Destination #Hyperrectangle
 * @param[in] region Source #AdversarialRegion
 */
static void region_to_hyperrectangle_l_inf(
    Hyperrectangle x,
    const AdversarialRegion region
) {
    unsigned int i;

    for (i = 0; i < region.space_size; ++i) {
        x->intervals[i].l = region.sample[i] - region.perturbation.data.l_inf.magnitude;
        x->intervals[i].u = region.sample[i] + region.perturbation.data.l_inf.magnitude;
    }
}



/**
 * Converts a clip \f$L_\infty\f$ #AdversarialRegion into a #Hyperrectangle.
 * 
 * @param[out] x Destination #Hyperrectangle
 * @param[in] region Source #AdversarialRegion
 */
static void region_to_hyperrectangle_l_inf_clip_all(
    Hyperrectangle x,
    const AdversarialRegion region
) {
    unsigned int i;

    for (i = 0; i < region.space_size; ++i) {
        x->intervals[i].l = max(
            region.sample[i] - region.perturbation.data.l_inf_clip_all.magnitude,
            region.perturbation.data.l_inf_clip_all.min
        );
        x->intervals[i].u = min(
            region.sample[i] + region.perturbation.data.l_inf_clip_all.magnitude,
            region.perturbation.data.l_inf_clip_all.max
        );
    }
}



/**
 * Converts a clip \f$L_\infty\f$ #AdversarialRegion into a #Hyperrectangle.
 * 
 * @param[out] x Destination #Hyperrectangle
 * @param[in] region Source #AdversarialRegion
 */
static void region_to_hyperrectangle_from_file(
    Hyperrectangle x,
    const AdversarialRegion region
) {
    unsigned int i;

    for (i = 0; i < region.space_size; ++i) {
        unsigned int n = fscanf(region.perturbation.data.from_file.stream, "[%lf;%lf] ", &x->intervals[i].l, &x->intervals[i].u);
        if (n != 2) {
            fprintf(stderr, "[%s: %d] Error while reading perturbation on dimension %d.", __FILE__, __LINE__, i);
            abort();
        }
    }
}



/**
 * Converts an AdversarialRegion into a #Hyperrectangle.
 * 
 * @param[out] x Destination #Hyperrectangle
 * @param[in] region Source #AdversarialRegion
 */
static void adversarial_region_to_hyperrectangle(
    Hyperrectangle x,
    const AdversarialRegion region
) {
    switch (region.perturbation.type) {
        case PERTURBATION_L_INF:
            region_to_hyperrectangle_l_inf(x, region);
            break;

        case PERTURBATION_L_INF_CLIP_ALL:
            region_to_hyperrectangle_l_inf_clip_all(x, region);
            break;

        case PERTURBATION_FROM_FILE:
            region_to_hyperrectangle_from_file(x, region);
            break;
    }
}



/***********************************************************************
 * Public functions.
 **********************************************************************/

void classifier_hyperrectangle_is_stable(
    StabilityStatus *result,
    const Classifier C,
    const AdversarialRegion x,
    const Tier t
) {
    Hyperrectangle h;

    hyperrectangle_create(&h, classifier_get_feature_space_size(C));
    adversarial_region_to_hyperrectangle(h, x);

    switch (classifier_get_type(C)) {
    case CLASSIFIER_TREE:
        decision_tree_hyperrectangle_is_stable(
            result,
            classifier_get_decision_tree(C),
            h,
            t
        );
        break;

    case CLASSIFIER_FOREST:
        forest_hyperrectangle_is_stable(
            result,
            classifier_get_forest(C),
            h,
            t
        );
        break;
    }

    hyperrectangle_delete(&h);
}
