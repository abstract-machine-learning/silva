/**
 * Defines a stopwatch to measure CPU time.
 *
 * @file stopwatch.h
 * @author Marco Zanella <marco.zanella.1991@gmail.com>
 */
#ifndef STOPWATCH_H
#define STOPWATCH_H

/** Type of a stopwatch. */
typedef struct stopwatch *Stopwatch;


/**
 * Creates a stopwatch.
 *
 * @param[out] S Pointer to stopwatch to create
 * @warning #stopwatch_delete should be called to ensure proper memory
 *          deallocation.
 */
void stopwatch_create(Stopwatch *S);


/**
 * Deletes a stopwatch.
 *
 * @param[out] S Pointer to stopwatch to delete
 */
void stopwatch_delete(Stopwatch *S);



/**
 * Returns elapsed time in seconds.
 *
 * @param[in] S Stopwatch
 * @return Elapsed time, in seconds
 */
double stopwatch_get_elapsed_time_seconds(const Stopwatch S);


/**
 * Returns elapsed time in milliseconds.
 *
 * @param[in] S Stopwatch
 * @return Elapsed time, in milliseconds
 */
double stopwatch_get_elapsed_time_milliseconds(const Stopwatch S);



/**
 * Resets a stopwatch.
 *
 * @param[out] S Stopwatch to reset
 */
void stopwatch_reset(Stopwatch S);


/**
 * Starts a stopwatch.
 *
 * If stopwatch was paused, previous time is accumulated and not lost.
 *
 * @param[in,out] S Stopwatch to start
 */
void stopwatch_start(Stopwatch S);


/**
 * Pauses a stopwatch.
 *
 * @param[out] S Stopwatch to pause
 */
void stopwatch_pause(Stopwatch S);


/**
 * Stops a stopwatch.
 *
 * @param[out] S Stopwatch to stop
 */
void stopwatch_stop(Stopwatch S);

#endif
