/* CS 6210 - Fall 2017
 * Project - 2
 * MCS Barrier - Test
 * Palak Choudhary
 */

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <sys/time.h>
# include <float.h>  //For DBL_MAX

# include "mcs.h"

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
   int NUM_PROCESSES;
   int i;

   /*
    * Variables needed for timing measurements
    */
   double process_start_time;  //Entry time of thread in barrier
   double process_end_time; //Exit time of thread from barrier
   double process_total_time_spent;  //Time spent by thread in a barrier
   double barrier_start_time; //Start time of a barrier
   double barrier_end_time;   //End time of a barrier
   double barrier_avg_time_spent;   //Average time a thread spent in a barrier
   double barrier_total_time_spent; //Total time spent in a barrier
   double overall_avg_time_spent;   //Average of average time a thread spent in a barrier
   double overall_total_time_spent; //Average of total time spent in a barrier

   /*
    * Variables needed for algorithm logic
    */
   int rank;
   bool sense;
   treenode *nodes;
   
   /*
    * Getting command-line arguments for number of barriers and number of threads
    */
   if (argc != 2)
   {
      printf("Error, invalid number of arguments\nProper usage: mpirun ./mcs <number of barriers>\nExiting\n");
      exit(-1);   //Improper call, so exit the program
   }
   else
   {
      NUM_BARRIERS = atoi(argv[1]);
   }

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &NUM_PROCESSES);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
   
   nodes = (treenode *) malloc(NUM_PROCESSES * sizeof(treenode));

   mcs_init(nodes, NUM_PROCESSES, rank, &sense);  //Set initial state of nodes 

   overall_avg_time_spent = 0;   //Initial value
   overall_total_time_spent = 0; //Initial value
   
   barrier_avg_time_spent = 0;   //Initial value
   barrier_start_time = DBL_MAX; //Maximum value for comparison
   barrier_end_time = 0;   //Minimum value for comparison
   
   for(i = 0; i < NUM_BARRIERS; i++)
   {
      process_start_time = mysecond();
      mcs_barrier(nodes, NUM_PROCESSES, i, rank, &sense);
      process_end_time = mysecond();
      
      process_total_time_spent = process_end_time - process_start_time;
      printf("Process %d spent %f in barrier %d\n", rank, process_total_time_spent, i);
      
      MPI_Reduce(&process_start_time, &barrier_start_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
      MPI_Reduce(&process_end_time, &barrier_end_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
      MPI_Reduce(&process_total_time_spent, &barrier_avg_time_spent, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
      
      if (rank == 0)
      {
         barrier_total_time_spent = barrier_end_time - barrier_start_time;
         barrier_avg_time_spent /= NUM_PROCESSES;

         printf("\nTotal time spent in barrier %d (in seconds): %f\n", i, barrier_total_time_spent);
         printf("Average time spent by a thread in barrier %d (in seconds): %f\n\n", i, barrier_avg_time_spent);

         overall_avg_time_spent += barrier_avg_time_spent;
         overall_total_time_spent += barrier_total_time_spent;

         /*
          * Restting values
          */
         barrier_avg_time_spent = 0;   //Initial value
         barrier_start_time = DBL_MAX; //Maximum value for comparison
         barrier_end_time = 0;   //Minimum value for comparison
      }
   }

   if (rank == 0)
   {
      overall_avg_time_spent /= NUM_BARRIERS;
      overall_total_time_spent /= NUM_BARRIERS;

      printf("Overall average time spent by a thread in a barrier (in seconds): %f\n", overall_avg_time_spent);
      printf("Overall average time spent in a barrier: %f\n", overall_total_time_spent);
   }
   
   MPI_Finalize();
   return 0;
}
