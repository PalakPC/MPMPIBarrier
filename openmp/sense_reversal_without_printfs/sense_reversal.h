/* CS 6210 - Fall 2017
 * Project - 2
 * Sense Reversing Barrier - Header
 * Palak Choudhary
 */

# include <omp.h>    //For OpenMP

void sense_reversal_init(int NUM_THREADS, int *count, bool *sense);

void sense_reversal_barrier(int thread_num, int NUM_THREADS, int barrier, int *count, bool *sense, bool *local_sense);
