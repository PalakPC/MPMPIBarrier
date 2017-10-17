/* CS 6210 - Fall 2017
 * Project - 2  
 * MCS Dissemination Combined Barrier - Header
 * Palak Choudhary
 */

# include "mcs.h"
# include "dissemination.h"

void combined_init(int NUM_THREADS, flags *allnodes, treenode *nodes, int NUM_PROCESSES, int rank, bool *process_sense);

void combined_barrier(int thread_num, int NUM_THREADS, flags *allnodes, int i, bool *thread_sense, int *parity, treenode *nodes, int NUM_PROCESSES, int rank, bool *process_sense);
