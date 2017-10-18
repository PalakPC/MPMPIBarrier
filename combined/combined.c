/* CS 6210 - Fall 2017
 * Project - 2  
 * MCS Dissemination Combined Barrier - Header
 * Palak Choudhary
 */

# include <stdio.h>
# include <stdbool.h>

# include "combined.h"

void combined_init(int NUM_THREADS, flags *allnodes, treenode *nodes, int NUM_PROCESSES, int rank, bool *process_sense, int *count, bool *sense)
{
   *count = NUM_THREADS;
   *sense = true; 
   dissemination_init(NUM_THREADS, allnodes);
   mcs_init(nodes, NUM_PROCESSES, rank, process_sense);
}

void combined_barrier(int thread_num, int NUM_THREADS, flags *allnodes, int i, bool *thread_sense, int *parity, treenode *nodes, int NUM_PROCESSES, int rank, bool *process_sense, int *count, bool *sense, bool*local_sense)
{
   int flag; //To check whether if was executed or not

   printf("Thread %d process %d in barrier %d\n", thread_num, rank, i);

   flag = 0;   //Initial value
   *local_sense = !(*local_sense);  //Toggle local sense
   
   dissemination_barrier(thread_num, NUM_THREADS, allnodes, i, thread_sense, parity);  //Thread-level barrier

   /*
    * After the thread-level barrier, threads still exist. 
    * They will go out of sync by the time the process-level barrier is done. 
    * Undesirable
    * Thus, one of the threads would initiate the process-level barrier.
    * This call will be inside a sense-reversal barrier 
    * to ensure that threads don't exit the function before processes hit their barrier.
    * 
    * Thus the functionality of the barrier is as follows:
    * *All processes sync their threads through dissemination.
    * *Last thread calls process-level barrier in each process.
    * *Others wait for the process-level barrier to be done.
    * *After that all threads across all processes exit the function together, happily.
    * *Barrier finished.
    */

#  pragma omp atomic
      (*count) -= 1;

   /*
    * Critical is non-blocking, threads go and wait in following if
    */  
#  pragma omp critical  //'cause we want only one thread to check if at a time
   {
      if (*count == 0)
      {
         mcs_barrier(nodes, NUM_PROCESSES, i, rank, process_sense);  //Process-level barrier
         *count = NUM_THREADS;   //Preparation for next call
         *sense = *local_sense;  //To inform others and kind of preparation for next call
         flag = 1;   //So that the thread who did this doesn't go in while
      }
   }

   if (flag == 0)
   {
      while (*sense != *local_sense);  //Keep waiting
   }

   printf("Thread %d process %d out of barrier %d\n", thread_num, rank, i);
}
