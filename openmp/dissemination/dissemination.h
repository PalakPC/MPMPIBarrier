/* CS 6210 - Fall 2017
 * Project - 2
 * Dissemination Barrier - Header
 * Palak Choudhary
 */

# include <omp.h>    //For OpenMP
# include <math.h>   //For log function

# define MAX_P 8
# define MAX_LOGP 3  //Maximum number of threads is 8, so maximum log2(P) will be 3

typedef struct
{
   int myflags[2][MAX_LOGP];
   int *partnerflags[2][MAX_LOGP];
} flags;

void dissemination_init(int NUM_THREADS, flags *allnodes);

void dissemination_barrier(int t, int NUM_THREADS, flags *allnodes, int barrier, int *sense, int *parity);
