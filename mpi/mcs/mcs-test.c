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

# define W_ARY 2
# define A_ARY 4

typedef struct
{
   double avg_time_spent;
   double total_time_spent;
} Measures;

typedef struct
{
   int parentsense;
   int *parentpointer;
   int *childpointers[W_ARY];
   int havechild[A_ARY];
   int childnotready[A_ARY];
   int dummy;
} treenode;

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
   int sense;
   treenode *nodes;
   
   double start_time;
   double end_time;
   double time_diff;
   double global_start_time;
   double global_end_time;
   double global_time_diff;
   double total_time;
   double overall_total_time;
   double overall_avg_time;
   
   /*
    * Getting command-line arguments for number of barriers and number of threads
    */
   if (argc != 2)
   {
      printf("Error, invalid number of arguments\nProper usage: ./dissemination <number of barriers> <number of threads>\nExiting\n");
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

   mcs_init(nodes, NUM_PROCESSES, rank);  //Set initial state of nodes 

   overall_avg_time_spent = 0;   //Initial value
   overall_total_time_spent = 0; //Initial value
   
   overall_avg_time = 0;
   overall_total_time = 0;

   for(i = 0; i < BARRIER_NUM; ++i)
   {
      start_time = mysecond();
      printf("\nProcess %d in barrier %d\n", rank, i);
      mcs_barrier(nodes, proc_num, i, rank, *sense);
      printf("Process %d out of barrier %d\n", rank, i);
      end_time = mysecond();
      
      time_diff = end_time - start_time;
      printf("Process %d spent %f in barrier %d\n", rank, time_diff, i);
      
      MPI_Reduce(&start_time, &global_start_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
      MPI_Reduce(&end_time, &global_end_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
      MPI_Reduce(&time_diff, &global_time_diff, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
      
      if (rank == 0)
      {
         total_time = global_end_time - global_start_time;
         overall_total_time += total_time;
         printf("\nTotal time spent in barrier %d: %f\n", i, total_time);
         global_time_diff /= proc_num;
         overall_avg_time += global_time_diff;
         printf("Average time spent by a thread in barrier %d: %f\n\n", i, global_time_diff);
      }
   }

   if (rank == 0)
   {
      overall_avg_time /= BARRIER_NUM;
      overall_total_time /= BARRIER_NUM;

      printf("Overall average time spent by a process in a barrier: %f\n", overall_avg_time);
      printf("Overall average time spent in a barrier: %f\n", overall_total_time);
   }
   
   MPI_Finalize();
   return 0;
}
