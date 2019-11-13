/**
 * Implements an abstract classifier.
 *
 * @file abstract_classifier.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "abstract_classifier.h"

#include <stdlib.h>

#include "classifier_hyperrectangle.h"


/** Structure of an abstract classifier. */
struct abstract_classifier {
    Classifier C;      /**< Concrete classifier. */
    AbstractDomain A;  /**< Abstract domain. */
};



void abstract_classifier_create(
    AbstractClassifier *AC,
    const Classifier C,
    const AbstractDomain A
) {
    AbstractClassifier ac = (AbstractClassifier) malloc(sizeof(struct abstract_classifier));
    if (ac == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    ac->C = C;
    ac->A = A;
    *AC = ac;
}



void abstract_classifier_delete(AbstractClassifier *AC) {
    free(*AC);
    *AC = NULL;
}



void abstract_classifier_is_stable(
    StabilityStatus *result,
    const AbstractClassifier AC,
    const AdversarialRegion x
) {
    if (AC == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    Classifier C = AC->C;

    switch (AC->A.type) {
    case DOMAIN_INTERVAL:
        fprintf(stderr, "[%s: %d] Cannot use interval abstract domain.\n", __FILE__, __LINE__);
        abort();

    case DOMAIN_HYPERRECTANGLE:
        classifier_hyperrectangle_is_stable(result, C, x);
        break;
    }
}



void abstract_classifier_print(
    const AbstractClassifier AC,
    FILE *stream
) {
    fprintf(stream, "Abstract classifier @%p.\n", (void *) AC);
    fprintf(stream, "Concrete classifier:\n");
    classifier_print(AC->C, stream);
    fprintf(stream, "Abstract domain:\n");
    abstract_domain_print(AC->A, stream);
}
