/* CS 6210 - Fall 2017
 * Project - 2
 * MCS Barrier - Header
 * Palak Choudhary
 */

# include <mpi.h>
# include <math.h>

# define W_ARY 2
# define A_ARY 4

#define MPI_CHK(err) if (err != MPI_SUCCESS) return err

typedef struct
{
   bool parentsense;
   bool *parentpointer;
   bool *childpointers[W_ARY];
   bool havechild[A_ARY];
   bool childnotready[A_ARY];
   bool dummy;
} treenode;

void mcs_init(treenode *nodes, int NUM_PROCESSES, int i, bool *sense);

void mcs_barrier(treenode *nodes, int NUM_PROCESSES, int barrier, int rank, bool *sense);
