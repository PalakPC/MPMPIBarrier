/* CS 6210 - Fall 2017
 * Project - 2
 * Default Barrier - Test
 * Palak Choudhary
 */

# include <stdio.h>
# include <stdlib.h> //For exit()
# include <stdbool.h>   //For boolean variables
# include <mpi.h>

#define MPI_CHK(err) if (err != MPI_SUCCESS) return err
# include "mytime.h"

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
   unsigned long long process_start_time;  //Entry time of thread in barrier
   unsigned long long process_end_time; //Exit time of thread from barrier
   unsigned long long process_total_time_spent;  //Time spent by thread in a barrier
   unsigned long long recieve_buffer;
   long double barrier_avg_time_spent;   //Average time a thread spent in a barrier
   long double overall_avg_time_spent;   //Average of average time a thread spent in a barrier

   /*
    * Variables needed for algorithm logic
    */
   int rank;
   
   /*
    * Getting command-line arguments for number of barriers and number of threads
    */
   if (argc != 2)
   {
      printf("Error, invalid number of arguments\nProper usage: mpirun -np <number of processes> ./baseline <number of barriers>\nExiting\n");
      exit(-1);   //Improper call, so exit the program
   }
   else
   {
      NUM_BARRIERS = atoi(argv[1]);
   }

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &NUM_PROCESSES);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
   
   overall_avg_time_spent = 0;   //Initial value
   
   barrier_avg_time_spent = 0;   //Initial value
   
   for(i = 0; i < NUM_BARRIERS; i++)
   {
      process_start_time = mysecond();
      
      int err = MPI_Barrier(MPI_COMM_WORLD); MPI_CHK(err);
      
      process_end_time = mysecond();
      
      process_total_time_spent = process_end_time - process_start_time;
      printf("%d\t%Lu\t%Lu\t%Lu\n", rank, process_total_time_spent, process_end_time, process_start_time);
      
      MPI_Reduce(&process_total_time_spent, &recieve_buffer, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
      
      if (rank == 0)
      {
         barrier_avg_time_spent = recieve_buffer;
         barrier_avg_time_spent /= NUM_PROCESSES;

         printf("Average time spent by a process in barrier %d (in nanoseconds): %Lf\n\n", i, barrier_avg_time_spent);

         overall_avg_time_spent += barrier_avg_time_spent;
         
         /*
          * Restting values
          */
         barrier_avg_time_spent = 0;   //Initial value
      }
   }

   if (rank == 0)
   {
      overall_avg_time_spent /= NUM_BARRIERS;

      printf("Overall average time spent by a process in a barrier (in nanoseconds): %Lf\n", overall_avg_time_spent);
   }
   
   MPI_Finalize();
   return 0;
}
