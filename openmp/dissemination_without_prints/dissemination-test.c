/* CS 6210 - Fall 2017
 * Project - 2
 * Dissemination Barrier - Test
 * Palak Choudhary
 */

# include <stdio.h>
# include <stdlib.h> //For exit()
# include <stdbool.h>   //For boolean datatype
# include <sys/time.h>  //For gettimeofday()
//# include <float.h>  //For DBL_MAX

# include "dissemination.h"   //Header

double mysecond()
{
   struct timeval tp;
   gettimeofday(&tp, NULL);
   return ((double) tp.tv_sec + (double) tp.tv_usec / 1.e6);
}

int main(int argc, char **argv)
{
   /*
    * Variables needed for code operation
    */
   int NUM_BARRIERS;
   int NUM_THREADS;

   /*
    * Variables needed for timing measurements
    */
   int counter;   //Thread counter
   double barrier_avg_time_spent;   //Average time a thread spent in a barrier
   double overall_avg_time_spent;   //Average of average time a thread spent in a barrier
//   double barrier_start_time; //Start time of a barrier
//   double barrier_end_time;   //End time of a barrier
//   double barrier_total_time_spent; //Total time spent in a barrier
//   double overall_total_time_spent; //Average of total time spent in a barrier

   /*
    * Variables needed for algorithm logic
    */
   flags *allnodes;  //Shared across threads

   /*
    * Getting command-line arguments for number of barriers and number of threads
    */
   if (argc != 3)
   {
      printf("Error, invalid number of arguments\nProper usage: ./dissemination <number of barriers> <number of threads>\nExiting\n");
      exit(-1);   //Improper call, so exit the program
   }
   else
   {
      NUM_BARRIERS = atoi(argv[1]);
      NUM_THREADS = atoi(argv[2]);
   }
   
   allnodes = (flags *) malloc(NUM_THREADS * sizeof(flags));

   dissemination_init(NUM_THREADS, allnodes);   //Set initial state of allnodes

   overall_avg_time_spent = 0;   //Initial value
//   overall_total_time_spent = 0; //Initial value
   
   counter = 0;
   barrier_avg_time_spent = 0;   //Initial value
//   barrier_start_time = DBL_MAX; //Maximum value for comparison
//   barrier_end_time = 0;   //Minimum value for comparison

   omp_set_num_threads(NUM_THREADS);   //Setting number of threads for parallel section

#  pragma omp parallel shared(allnodes) //Parallel region start
   {
      /*
       * variables private to each thread
       */
      int i;   //for barrier loop
      int thread_num;
      int parity;
      bool sense;

      /*
       * Private timing variables
       */
      double thread_start_time;  //Entry time of thread in barrier
      double thread_end_time; //Exit time of thread from barrier
      double thread_total_time_spent;  //Time spent by thread in a barrier

      thread_num = omp_get_thread_num();  //To get thread ID
      parity = 0; //Initial value
      sense = true;  //Initial value

      for (i = 0; i < NUM_BARRIERS; i++)
      {
         thread_start_time = mysecond();  //Starting time of this thread

//#        pragma omp critical  //Shared variable, need critical to ensure correctness
//         {
//            if (thread_start_time < barrier_start_time)
//            {
//               barrier_start_time = thread_start_time;   //To get the time when first thread entered the barrier
//            }
//         }

         dissemination_barrier(thread_num, NUM_THREADS, &allnodes[thread_num], i, &sense, &parity); //Call to barrier function

         thread_end_time = mysecond();

//#        pragma omp critical  //Shared variable, need critical to ensure correctness
//         {
//            if (thread_end_time > barrier_end_time)
//            {
//               barrier_end_time = thread_end_time; //To get the time when last thread exited the barrier
//            }
//         }

         thread_total_time_spent = thread_end_time - thread_start_time;
//         printf("%d\t%f\t%f\t%f\n", thread_num, thread_total_time_spent, thread_end_time, thread_start_time);

#        pragma omp atomic //Atomic operation to ensure correctness
            barrier_avg_time_spent += thread_total_time_spent;

#        pragma omp critical
         {
            counter++;
            if (counter == NUM_THREADS)
            {
//               barrier_total_time_spent = barrier_end_time - barrier_start_time;
               barrier_avg_time_spent /= NUM_THREADS;

//               printf("\nTotal time spent in barrier %d (in seconds): %f\n", i, barrier_total_time_spent);
//               printf("Average time spent by a thread in barrier %d (in seconds): %f\n\n", i, barrier_avg_time_spent);

               overall_avg_time_spent += barrier_avg_time_spent;
//               overall_total_time_spent += barrier_total_time_spent;

               /*
                * Restting values
                */
               counter = 0;
               barrier_avg_time_spent = 0;   //Initial value
//               barrier_start_time = DBL_MAX; //Maximum value for comparison
//               barrier_end_time = 0;   //Minimum value for comparison
            }
         } 
      }  //All barriers done
   }  //Parallel region end

   overall_avg_time_spent /= NUM_BARRIERS;
//   overall_total_time_spent /= NUM_BARRIERS;

   printf("Overall average time spent by a thread in a barrier (in seconds) for threads\t%d\t:\t%f\n", NUM_THREADS, overall_avg_time_spent);
//   printf("Overall average time spent in a barrier: %f\n", overall_total_time_spent);

   return 0;
}
