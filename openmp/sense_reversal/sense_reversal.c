/* CS 6210 - Fall 2017
 * Project - 2
 * Sense Reversing Barrier
 * Palak Choudhary
 */

# include <stdio.h>
# include <stdbool.h>   //For boolean variables

# include "sense_reversal.h"

void sense_reversal_init(int NUM_THREADS, int *count, bool *sense)
{
   /*
    * Initialization of sense-reversing barrier
    */

   *count = NUM_THREADS;
   *sense = true;
}

void sense_reversal_barrier(int thread_num, int NUM_THREADS, int barrier, int *count, bool *sense, bool *local_sense)
{
   /*
    * Implementation of sense-reversing barrier
    */

   int flag; //To check whether if was executed or not

   flag = 0;
   *local_sense = !(*local_sense);

//   printf("Thread %d in barrier %d\n", thread_num, barrier);

#  pragma omp atomic
      (*count) -= 1;

#  pragma omp critical
   {
      if (*count == 0)
      {
         *count = NUM_THREADS;
         *sense = *local_sense;
         flag = 1;
      }
   }

   if (flag == 0)
   {
      while (*sense != *local_sense);
   }

//   printf("Thread %d out of barrier %d\n", thread_num, barrier);
}
