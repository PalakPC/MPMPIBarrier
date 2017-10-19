/* CS 6210 - Fall 2017
 * Project - 2  
 * MCS Dissemination Combined Barrier - Test
 * Palak Choudhary
 */

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <sys/time.h>
# include <float.h>  //for DBL_MAX

# include "combined.h"

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
   int NUM_PROCESSES;

   /*
    * Variables needed for timing measurements
    */
   int counter;

   double process_start_time;  //Entry time of thread in barrier
   double process_end_time; //Exit time of thread from barrier
   double process_total_time_spent;  //Time spent by thread in a barrier
   double barrier_process_start_time; //Start time of a barrier
   double barrier_process_end_time;   //End time of a barrier
   double barrier_process_avg_time_spent;   //Average time a thread spent in a barrier
   double barrier_process_total_time_spent; //Total time spent in a barrier
   double overall_process_avg_time_spent;   //Average of average time a thread spent in a barrier
   double overall_process_total_time_spent; //Average of total time spent in a barrier

   double barrier_thread_start_time; //Start time of a barrier
   double barrier_thread_end_time;   //End time of a barrier
   double barrier_thread_avg_time_spent;   //Average time a thread spent in a barrier
   double barrier_thread_total_time_spent; //Total time spent in a barrier
   double overall_thread_avg_time_spent;   //Average of average time a thread spent in a barrier
   double overall_thread_total_time_spent; //Average of total time spent in a barrier

   /*
    * Variables needed for algorithm logic
    */
   int rank;
   bool process_sense;
   treenode *nodes;
   flags *allnodes;  //Shared across threads
   int count;
   bool sense;

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

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &NUM_PROCESSES);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);  

   nodes = (treenode *) malloc(NUM_PROCESSES * sizeof(treenode));
   allnodes = (flags *) malloc(NUM_THREADS * sizeof(flags));

   combined_init(NUM_THREADS, allnodes, nodes, NUM_PROCESSES, rank, &process_sense, &count, &sense);   //Set initial state of allnodes

   overall_thread_avg_time_spent = 0;   //Initial value
   overall_thread_total_time_spent = 0; //Initial value

   overall_process_avg_time_spent = 0;   //Initial value
   overall_process_total_time_spent = 0; //Initial value

   counter = 0;
   barrier_thread_avg_time_spent = 0;   //Initial value
   barrier_thread_start_time = DBL_MAX; //Maximum value for comparison
   barrier_thread_end_time = 0;   //Minimum value for comparison

   barrier_process_avg_time_spent = 0;   //Initial value
   barrier_process_start_time = DBL_MAX; //Maximum value for comparison
   barrier_process_end_time = 0;   //Minimum value for comparison

   omp_set_num_threads(NUM_THREADS);   //Setting number of threads for parallel section

#  pragma omp parallel shared(allnodes) //Parallel region start
   {
      /*
       * variables private to each thread
       */
      int i;   //for barrier loop
      int thread_num;
      int parity;
      bool thread_sense;
      bool local_sense;

      int flag;   //Needed to check last thread

      /*
       * Private timing variables
       */
      double thread_start_time;  //Entry time of thread in barrier
      double thread_end_time; //Exit time of thread from barrier
      double thread_total_time_spent;  //Time spent by thread in a barrier

      thread_num = omp_get_thread_num();  //To get thread ID
      parity = 0; //Initial value
      thread_sense = true;  //Initial value
      local_sense = true;  //Initial value

      for (i = 0; i < NUM_BARRIERS; i++)
      {
         thread_start_time = mysecond();  //Starting time of this thread
         combined_barrier(thread_num, NUM_THREADS, &allnodes[thread_num], i, &thread_sense, &parity, nodes, NUM_PROCESSES, rank, &process_sense, &count, &sense, &local_sense); //Call to barrier function
         thread_end_time = mysecond();

#        pragma omp critical  //Shared variable, need critical to ensure correctness
         {
            if (thread_start_time < barrier_thread_start_time)
            {
               barrier_thread_start_time = thread_start_time;   //To get the time when first thread entered the barrier
            }
         }

#        pragma omp critical  //Shared variable, need critical to ensure correctness
         {
            if (thread_end_time > barrier_thread_end_time)
            {
               barrier_thread_end_time = thread_end_time; //To get the time when last thread exited the barrier
            }
         }

         thread_total_time_spent = thread_end_time - thread_start_time;
//         printf("%d\t%f\t%f\t%f\n", thread_num, thread_total_time_spent, thread_end_time, thread_start_time);

#        pragma omp atomic //Atomic operation to ensure correctness
         barrier_thread_avg_time_spent += thread_total_time_spent;

         process_start_time = barrier_thread_start_time;
         process_end_time = barrier_thread_end_time;

#        pragma omp critical
         {
            counter++;
            if (counter == NUM_THREADS)
            {
               flag = 1;
            }
         }

         if (flag == 1)
         {
            barrier_thread_total_time_spent = barrier_thread_end_time - barrier_thread_start_time;
            barrier_thread_avg_time_spent /= NUM_THREADS;

//            printf("\nTotal time spent in barrier %d (in seconds): %f\n", i, barrier_thread_total_time_spent);
//            printf("Average time spent by a thread in barrier %d (in seconds): %f\n\n", i, barrier_thread_avg_time_spent);

            overall_thread_avg_time_spent += barrier_thread_avg_time_spent;
            overall_thread_total_time_spent += barrier_thread_total_time_spent;

            /*
             * Restting values
             */
            counter = 0;
            barrier_thread_avg_time_spent = 0;   //Initial value
            barrier_thread_start_time = DBL_MAX; //Maximum value for comparison
            barrier_thread_end_time = 0;   //Minimum value for comparison

            process_total_time_spent = process_end_time - process_start_time;
//            printf("%d\t%f\t%f\t%f\n", rank, process_total_time_spent, process_end_time, process_start_time);

            MPI_Reduce(&process_start_time, &barrier_process_start_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
            MPI_Reduce(&process_end_time, &barrier_process_end_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
            MPI_Reduce(&process_total_time_spent, &barrier_process_avg_time_spent, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

            if (rank == 0)
            {
               barrier_process_total_time_spent = barrier_process_end_time - barrier_process_start_time;
               barrier_process_avg_time_spent /= NUM_PROCESSES;

//               printf("\nTotal time spent in barrier %d (in seconds): %f\n", i, barrier_process_total_time_spent);
//               printf("Average time spent by a process in barrier %d (in seconds): %f\n\n", i, barrier_process_avg_time_spent);

               overall_process_avg_time_spent += barrier_process_avg_time_spent;
               overall_process_total_time_spent += barrier_process_total_time_spent;

               /*
                * Restting values
                */
               barrier_process_avg_time_spent = 0;   //Initial value
               barrier_process_start_time = DBL_MAX; //Maximum value for comparison
               barrier_process_end_time = 0;   //Minimum value for comparison
            }
         }
      }  //All barriers done
   }  //Parallel region end

   overall_thread_avg_time_spent /= NUM_BARRIERS;
   overall_thread_total_time_spent /= NUM_BARRIERS;

   printf("Overall average time spent by a thread in a barrier (in seconds): %f\n", overall_thread_avg_time_spent);
   printf("Overall average time spent in a barrier: %f\n", overall_thread_total_time_spent);

   if (rank == 0)
   {
      overall_process_avg_time_spent /= NUM_BARRIERS;
      overall_process_total_time_spent /= NUM_BARRIERS;

      printf("Overall average time spent by a process in a barrier (in seconds): %f\n", overall_process_avg_time_spent);
      printf("Overall average time spent in a barrier: %f\n", overall_process_total_time_spent);
   }

   MPI_Finalize();
   return 0;
}
