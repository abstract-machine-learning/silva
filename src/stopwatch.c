/**
 * Implements a stopwatch to measure CPU time.
 *
 * @file stopwatch.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#include "stopwatch.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/** Structure of a stopwatch. */
struct stopwatch {
    double elapsed_time;  /**< Elapsed time, in milliseconds. */
    clock_t start_clock;  /**< Start clock. */
};



void stopwatch_create(Stopwatch *S) {
    Stopwatch s = (Stopwatch) malloc(sizeof(struct stopwatch));
    if (s == NULL) {
        fprintf(stderr, "[%s: %d] Cannot allocate memory.\n", __FILE__, __LINE__);
        abort();
    }

    s->elapsed_time = 0.0;
    s->start_clock = clock();

    *S = s;
}



void stopwatch_delete(Stopwatch *S) {
    if (S == NULL || *S == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    free(*S);
    *S = NULL;
}



double stopwatch_get_elapsed_time_seconds(const Stopwatch S) {
    if (S == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    return S->elapsed_time;
}



double stopwatch_get_elapsed_time_milliseconds(const Stopwatch S) {
    return stopwatch_get_elapsed_time_seconds(S) * 1e3;
}



void stopwatch_reset(Stopwatch S) {
    if (S == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    S->elapsed_time = 0.0;
    S->start_clock = clock();
}



void stopwatch_start(Stopwatch S) {
    if (S == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    S->start_clock = clock();
}



void stopwatch_pause(Stopwatch S) {
    if (S == NULL) {
        fprintf(stderr, "[%s: %d] Unexpected NULL pointer.\n", __FILE__, __LINE__);
        abort();
    }

    S->elapsed_time += (double) (clock() - S->start_clock) / (double) CLOCKS_PER_SEC;
}



void stopwatch_stop(Stopwatch S) {
    stopwatch_pause(S);
}
