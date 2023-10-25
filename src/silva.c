/**
 * Silvarum Intervallis Lator Valens Analysis.
 * 
 * Verifies properties of random forests.
 * 
 * @file silva.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "options.h"
#include "data_mappers/classifier_silva.h"
#include "dataset.h"
#include "abstract_interpreters/abstract_classifier.h"
#include "stopwatch.h"


/** Minimum space to print labels. */
#define LABELS_MIN_SIZE 16



/**
 * Prints a set of labels.
 *
 * @param[in] labels Set of classes
 */
static void print_labels(const Set labels) {
    const unsigned int n_labels = set_get_cardinality(labels);
    unsigned int i;
    char **labels_array = set_get_elements_as_array(labels);

    for (i = 2 * n_labels - 1; i < LABELS_MIN_SIZE; ++i) {
        printf(" ");
    }
    for (i = 0; i < n_labels; ++i) {
        printf("%s", labels_array[i]);
        if (i + 1 < n_labels) {
            printf(",");
        }
    }
}



/**
 * Prints a string subject to length constraints.
 *
 * @param[in] string String to print
 * @param[in] options Options
 */
static void print_string(const char *string, const Options options) {
    const unsigned int length = strlen(string),
                       max_length = options.max_print_length - 3,
                       offset = length <= max_length
                              ? 0
                              : length - max_length;

    if (offset != 0) {
        printf("...");
    }
    printf("%s", string + offset);
}



/**
 * Main.
 * 
 * @param[in] argc ARGument Counter
 * @param[in] argv ARGument Vector
 * @return EXIT_SUCCESS in case of success, EXIT_FAILURE otherwise
 */
int main(const int argc, const char **argv) {
    unsigned int i;
    Options options;
    FILE *classifier_file, *dataset_file, *counterexamples_file = NULL;
    Dataset dataset;
    Classifier classifier;
    AbstractClassifier abstract_classifier;
    Set concrete_labels, abstract_labels;
    StabilityStatus status;
    Stopwatch stopwatch;
    double sample_time, previous_cumulative_time = 0.0;

    unsigned int n_correct = 0,
                 n_stable = 0,
                 n_unstable = 0,
                 n_robust = 0,
                 n_fragile = 0;


    /* Parses command-line arguments */
    if (argc < 3) {
        display_help(argc, argv);
        exit(EXIT_FAILURE);
    }
    options_read(&options, argc, argv);


    /* Reads dataset */
    dataset_file = fopen(options.dataset_path, "r");
    dataset = dataset_read(dataset_file);
    fclose(dataset_file);


    /* Reads classifier */
    classifier_file = fopen(options.classifier_path, "r");
    classifier_silva_read(&classifier, classifier_file);
    fclose(classifier_file);
    if (classifier_get_type(classifier) == CLASSIFIER_FOREST) {
        forest_set_voting_scheme(classifier_get_forest(classifier), options.voting_scheme);
    }


    /* Creates abstract classifier */
    abstract_classifier_create(&abstract_classifier, classifier, options.abstract_domain, &options.tier);


    /* Prepares auxiliary data structures */
    set_create(&concrete_labels, set_equality_string);
    set_create(&abstract_labels, set_equality_string);
    status.sample_b = malloc(classifier_get_feature_space_size(classifier) * sizeof(double));
    hyperrectangle_create(&status.region, classifier_get_feature_space_size(classifier));
    status.labels_a = concrete_labels;
    status.timeout = options.sample_timeout;
    stopwatch_create(&stopwatch);


    /* Opens counterexamples file, if necessary */
    if (options.counterexamples_path != NULL) {
        counterexamples_file = fopen(options.counterexamples_path, "w");
    }


    /* Prints heading */
    printf("%-*s %-*s %8s %8s %*s %10s %10s\n",
        options.max_print_length, "Classifier",
        options.max_print_length, "Dataset", 
        "ID",
        "Label",
        LABELS_MIN_SIZE, "Concrete",
        "Result",
        "Time (s)"
    );



    /* Analyses each sample */
    stopwatch_start(stopwatch);
    for (i = 0; i < dataset_get_size(dataset); ++i) {
        const double *sample = dataset_get_row(dataset, i);
        const char *label = dataset_get_label(dataset, i);
        const AdversarialRegion adversarial_region = {
            sample,
            classifier_get_feature_space_size(classifier),
            options.perturbation
        };
        stability_status_set_sample(&status, (double *) sample, concrete_labels);

        classifier_classify(concrete_labels, classifier, sample);
        abstract_classifier_is_stable(
            &status,
            abstract_classifier,
            adversarial_region
        );

        /* Computes statistics */
        const unsigned int is_correct = set_is_singleton(concrete_labels)
                                     && set_has_element(concrete_labels, label),
                           is_stable = status.result == STABILITY_TRUE,
                           is_unstable = status.result == STABILITY_FALSE;

        n_correct    += is_correct;
        n_stable     += is_stable;
        n_unstable   += is_unstable;
        n_robust     += is_correct && is_stable;
        n_fragile    += is_correct && is_unstable;


        /* Displays result */
        stopwatch_pause(stopwatch);
        sample_time = stopwatch_get_elapsed_time_seconds(stopwatch) - previous_cumulative_time;
        previous_cumulative_time = stopwatch_get_elapsed_time_seconds(stopwatch);
        print_string(options.classifier_path, options);
        printf(" ");
        print_string(options.dataset_path, options);
        printf(" ");
        printf("%8u %8s ", i, label);
        print_labels(concrete_labels);
        printf(" %10s",
            is_stable
            ? is_correct ? "ROBUST" : "VULNERABLE"
            : is_unstable
              ? is_correct ? "FRAGILE" : "BROKEN"
              : "NO-INFO"
        );
        printf(" %10g\n", sample_time);


        /* Exports counterexample, if necessary */
        if (counterexamples_file != NULL && is_unstable) {
            fprintf(counterexamples_file, "%d: ", i);
            hyperrectangle_dump(status.region, counterexamples_file);
        }


        /* Resumes stopwatch */
        stopwatch_start(stopwatch);
    }
    stopwatch_stop(stopwatch);


    /* Displays summary */
    printf(
        "[SUMMARY] %10s %10s %10s %10s %10s %10s %10s %10s %10s %12s %10s\n",
        "Size", "Time (s)", "Correct", "Wrong", "Stable", "Unstable",
        "No info", "Robust", "Fragile", "Vulnerable", "Broken"
    );
    printf(
        "[SUMMARY] %10u %10g %10u %10u %10u %10u %10u %10u %10u %12u %10u\n",
        dataset_get_size(dataset),
        stopwatch_get_elapsed_time_seconds(stopwatch),
        n_correct,
        dataset_get_size(dataset) - n_correct,
        n_stable,
        n_unstable,
        dataset_get_size(dataset) - n_stable - n_unstable,
        n_robust,
        n_fragile,
        n_stable - n_robust,
        n_unstable - n_fragile
    );


    /* Closes counterexamples file, if necessary */
    if (counterexamples_file != NULL) {
        fclose(counterexamples_file);
    }


    /* Deallocates memory */
    classifier_delete(&classifier);
    dataset_delete(&dataset);
    abstract_classifier_delete(&abstract_classifier);
    set_delete(&concrete_labels);
    set_delete(&abstract_labels);
    free(status.sample_b);
    hyperrectangle_delete(&status.region);
    options_delete(&options);
    stopwatch_delete(&stopwatch);

    return EXIT_SUCCESS;
}
