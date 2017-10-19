/* CS 6210 - Fall 2017
 * Project - 2
 * Sense Reversing Barrier - Test
 * Palak Choudhary
 */

# include <stdio.h>
# include <stdlib.h> //For exit()
# include <stdbool.h>   //For boolean variables

# include "sense_reversal.h"
# include "mytime.h"

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
   int counter;
   long double barrier_avg_time_spent;   //Average time a thread spent in a barrier
   long double overall_avg_time_spent;   //Average of average time a thread spent in a barrier

   /*
    * Variables needed for algorithm logic
    */
   int count;  //Shared across threads
   bool sense;  //Shared across threads
   
   /*
    * Getting command-line arguments for number of barriers and number of threads
    */
   if (argc != 3)
   {
      printf("Error, invalid number of arguments\nProper usage: ./sense_reversal <number of barriers> <number of threads>\nExiting\n");
      exit(-1);   //Improper call, so exit the program
   }
   else
   {
      NUM_BARRIERS = atoi(argv[1]);
      NUM_THREADS = atoi(argv[2]);
   }

   sense_reversal_init(NUM_THREADS, &count, &sense);  //Set initial state

   overall_avg_time_spent = 0;   //Initial value
   
   counter = 0;
   barrier_avg_time_spent = 0;   //Initial value

   omp_set_num_threads(NUM_THREADS);   //Setting number of threads for parallel section

#  pragma omp parallel shared(count, sense) //Parallel region start
   {
      /*
       * variables private to each thread
       */
      int i;   //for barrier loop
      int thread_num;
      bool local_sense;

      /*
       * Private timing variables
       */
      unsigned long long thread_start_time;  //Entry time of thread in barrier
      unsigned long long thread_end_time; //Exit time of thread from barrier
      unsigned long long thread_total_time_spent;  //Time spent by thread in a barrier

      thread_num = omp_get_thread_num();  //To get thread ID
      local_sense = true;  //Initial value

      for (i = 0; i < NUM_BARRIERS; i++)
      {

         thread_start_time = mysecond();  //Starting time of this thread

         sense_reversal_barrier(thread_num, NUM_THREADS, i, &count, &sense, &local_sense); //Call to barrier function

         thread_end_time = mysecond();

         thread_total_time_spent = thread_end_time - thread_start_time;
         printf("%d\t%Lu\t%Lu\t%Lu\n", thread_num, thread_total_time_spent, thread_end_time, thread_start_time);

#        pragma omp atomic //Atomic operation to ensure correctness
            barrier_avg_time_spent += thread_total_time_spent;

#        pragma omp critical
         {
            counter++;
            if (counter == NUM_THREADS)
            {
               barrier_avg_time_spent /= NUM_THREADS;

               printf("Average time spent by a thread in barrier %d (in nanoseconds): %Lf\n\n", i, barrier_avg_time_spent);

               overall_avg_time_spent += barrier_avg_time_spent;

               /*
                * Restting values
                */
               counter = 0;
               barrier_avg_time_spent = 0;   //Initial value
            }
         } 
      }  //All barriers done
   }  //Parallel region end

   overall_avg_time_spent /= NUM_BARRIERS;

   printf("Overall average time spent by a thread in a barrier (in nanoseconds): %Lf\n", overall_avg_time_spent);

   return 0;
}
