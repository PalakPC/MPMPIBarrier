/* CS 6210 - Fall 2017
 * Project - 2
 * Dissemination Barrier - Header
 * Palak Choudhary
 */

# include <omp.h>    //For OpenMP
# include <math.h>   //For log function

# define MAX_P 12
# define MAX_LOGP 4  //Changed from original dissemination because number of threads increased. Maximum number of threads is 12, so maximum log2(P) will be 4

typedef struct
{
   bool myflags[2][MAX_LOGP];
   bool *partnerflags[2][MAX_LOGP];
} flags;

void dissemination_init(int NUM_THREADS, flags *allnodes);

void dissemination_barrier(int t, int NUM_THREADS, flags *allnodes, int barrier, bool *sense, int *parity);
