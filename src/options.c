/**
 * Implements program options.
 *
 * Reads and sets program options.
 *
 * @file options.c
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "options.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/** Minimum number of characters to print */
#define MIN_PRINT_LENGTH 8

/** Default value of maximum number of characters to print */
#define MAX_PRINT_LENGTH 32

/** Default value for voting scheme */
#define VOTING_SCHEME FOREST_VOTING_MAX

/** Default timeout (per sample) */
#define SAMPLE_TIMEOUT 1

/** Default random seed */
#define SEED 42



/***********************************************************************
 * Internal functions.
 **********************************************************************/

/**
 * Reads voting scheme.
 *
 * @param[out] options Options
 * @param[in] argc ARGument Counter
 * @param[in] argv ARGument Vector
 * @param[in,out] i Pointer to current position in the argument vector
 */
static void read_voting_scheme(
    Options *options,
    const int argc,
    const char *argv[],
    int *i
) {
    (void) argc;

    if (strcmp(argv[*i], "max") == 0) {
        options->voting_scheme = FOREST_VOTING_MAX;
    }
    else if (strcmp(argv[*i], "average") == 0) {
        options->voting_scheme = FOREST_VOTING_AVERAGE;
    }
    else if (strcmp(argv[*i], "softargmax") == 0) {
        options->voting_scheme = FOREST_VOTING_SOFTARGMAX;
    }
    else {
        fprintf(stderr, "[%s: %d] Unsupported voting scheme.\n", __FILE__, __LINE__);
        abort();
    }
}



/**
 * Reads abstraction.
 *
 * @param[out] options Options
 * @param[in] argc ARGument Counter
 * @param[in] argv ARGument Vector
 * @param[in,out] i Pointer to current position in the argument vector
 */
static void read_abstraction(
    Options *options,
    const int argc,
    const char *argv[],
    int *i
) {
    (void) argc;

    if (strcmp(argv[*i], "interval") == 0) {
        options->abstract_domain.type = DOMAIN_INTERVAL;
    }
    else if (strcmp(argv[*i], "hyperrectangle") == 0) {
        options->abstract_domain.type = DOMAIN_HYPERRECTANGLE;
    }
    else {
        fprintf(stderr, "[%s: %d] Unsupported abstraction.\n", __FILE__, __LINE__);
        abort();
    }
}



/**
 * Reads perturbation and perturbation data.
 *
 * @param[out] options Options
 * @param[in] argc ARGumnet Counter
 * @param[in] argv ARGument Vector
 * @param[in,out] i Pointer to current position in the argument vector
 */
static void read_perturbation(
    Options *options,
    const int argc,
    const char *argv[],
    int *i
) {
    if (strcmp(argv[*i], "l_inf") == 0 && *i + 1 < argc) {
        options->perturbation.type = PERTURBATION_L_INF;
        ++*i;
        sscanf(argv[*i], "%lf", &options->perturbation.data.l_inf.magnitude);
    }

    else if (strcmp(argv[*i], "l_inf-clip-all") == 0 && *i + 3 < argc) {
        options->perturbation.type = PERTURBATION_L_INF_CLIP_ALL;
        sscanf(argv[*i + 1], "%lf", &options->perturbation.data.l_inf_clip_all.magnitude);
        sscanf(argv[*i + 2], "%lf", &options->perturbation.data.l_inf_clip_all.min);
        sscanf(argv[*i + 3], "%lf", &options->perturbation.data.l_inf_clip_all.max);
        *i += 3;
    }

    else {
        fprintf(stderr, "[%s: %d] Unsupported perturbation.\n", __FILE__, __LINE__);
        abort();
    }
}


/***********************************************************************
 * Public functions.
 **********************************************************************/

void options_read(Options *options, const int argc, const char *argv[]) {
    int i;

    options->classifier_path = (char *) argv[1];
    options->dataset_path = (char *) argv[2];
    options->max_print_length = MAX_PRINT_LENGTH;
    options->voting_scheme = VOTING_SCHEME;
    options->perturbation.type = PERTURBATION_L_INF;
    options->perturbation.data.l_inf.magnitude = 0.0;
    options->sample_timeout = SAMPLE_TIMEOUT;
    options->abstract_domain.type = DOMAIN_HYPERRECTANGLE;
    options->seed = SEED;

    for (i = 3; i < argc; ++i) {
        if (strcmp(argv[i], "--max-print-length") == 0 && i + 1 < argc) {
            ++i;
            sscanf(argv[i], "%u", &options->max_print_length);
            if (options->max_print_length < MIN_PRINT_LENGTH) {
                options->max_print_length = MIN_PRINT_LENGTH;
            }
        }
        else if (strcmp(argv[i], "--voting") == 0 && i + 1 < argc) {
            ++i;
            read_voting_scheme(options, argc, argv, &i);
        }
        else if (strcmp(argv[i], "--abstraction") == 0 && i + 1 < argc) {
            ++i;
            read_abstraction(options, argc, argv, &i);
        }
        else if (strcmp(argv[i], "--perturbation") == 0 && i + 1 < argc) {
            ++i;
            read_perturbation(options, argc, argv, &i);
        }
        else if (strcmp(argv[i], "--sample-timeout") == 0 && i + 1 < argc) {
            ++i;
            sscanf(argv[i], "%u", &options->sample_timeout);
        }
        else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            ++i;
            sscanf(argv[i], "%u", &options->seed);
        }
    }

    srand(options->seed);
}



void display_help(const int argc, const char *argv[]) {
    (void) argc;
    (void) argv;

    printf("Usage: %s <classifier> <dataset> [options]\n", argv[0]);
    printf("Verifies robustness of a decision tree or forest classifier on a dataset.\n\n");

    printf("Mandatory arguments:\n");
    printf("\t%-16s Path to classifier file, in silva format\n", "classifier");
    printf("\t%-16s Path to dataset file (CSV or binary)\n", "dataset");
    printf("\n");

    printf("Optional arguments:\n");
    printf("\t%-32s Maximum number of characters to print for long strings, -1 to disable limit (deafult: %u)\n", "--max-print-length VALUE", MAX_PRINT_LENGTH);
    printf("\t%-32s Voting scheme to use for forests (default: max)\n", "--voting {max | average | softargmax}");
    printf("\t%-32s Abstract domain to use (default: hyperrectangle)\n", "--abstraction {interval | hyperrectangle}");
    printf("\t%-32s Perturbation to analyse, followed by perturbation-specific options (default: l_inf 0)\n", "--perturbation {l_inf} [DATA]");
    printf("\t%-32s Maximum allowed execution time for each sample analysis, in seconds (default: %u)\n", "--sample-timeout VALUE", SAMPLE_TIMEOUT);
    printf("\t%-32s Seed to use for random number generation, reserved for future use (default: %u)\n", "--seed VALUE", SEED);
    printf("\n");

    printf("Perturbation-specific options:\n");
    printf("\tl_inf\n");
    printf("\t\tmagnitude\tRadius of the L_inf ball giving the perturbation region\n");
    printf("\n");

    printf("Examples:\n");
    printf("Analyses classifier \"my_classifier.silva\" using \"my_dataset.csv\", adversarial region is generated by an L_inf ball with radius 64, analysis is performed using intervals:\n");
    printf("\tsilva my_classifier.silva my_dataset.csv --abstraction interval --perturbation l_inf 64\n");
}



void options_print(const Options options, FILE *stream) {
    fprintf(stream, "Program options:\n");
    fprintf(stream, "\tclassifier path: %s\n", options.classifier_path);
    fprintf(stream, "\tdataset path: %s\n", options.dataset_path);
    fprintf(stream, "\tvoting scheme: %s\n", options.voting_scheme == FOREST_VOTING_MAX ? "max" : "average");
    fprintf(stream, "\tperturbation: ");
    perturbation_print(options.perturbation, stream);
    fprintf(stream, "\n");
    fprintf(stream, "\tsample timeout: %u\n", options.sample_timeout);
    fprintf(stream, "\tabstraction: ");
    abstract_domain_print(options.abstract_domain, stream);
    fprintf(stream, "\n");
    fprintf(stream, "\tseed: %u\n", options.seed);
}
