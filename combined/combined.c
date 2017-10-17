/* CS 6210 - Fall 2017
 * Project - 2  
 * MCS Dissemination Combined Barrier - Header
 * Palak Choudhary
 */

# include <stdio.h>
# include <stdbool.h>

# include "combined.h"

void combined_init(int NUM_THREADS, flags *allnodes, treenode *nodes, int NUM_PROCESSES, int rank, bool *process_sense)
{
   dissemination_init(NUM_THREADS, allnodes);
   mcs_init(nodes, NUM_PROCESSES, rank, process_sense);
}

void combined_barrier(int thread_num, int NUM_THREADS, flags *allnodes, int i, bool *thread_sense, int *parity, treenode *nodes, int NUM_PROCESSES, int rank, bool *process_sense)
{
   dissemination_barrier(thread_num, NUM_THREADS, allnodes, i, thread_sense, parity);

#  pragma omp critical
//   if (thread_num == 0)
   {
      mcs_barrier(nodes, NUM_PROCESSES, i, rank, process_sense);
//      *sync = 1;
   }
//   else
//   {
//      while (*sync == 0);
//   }
}
