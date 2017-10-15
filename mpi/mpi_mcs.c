/* CS 6210 - Fall 2017
 * MCS Barrier
 * Palak Choudhary
 */

# include <mpi.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <math.h>
# include <unistd.h>

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

void mcs_barrier(treenode *nodes, int proc_num, int barrier, int rank, int sense)
{
   int i;
   int buffer;
   int buffer2;
   int floor_val;
   MPI_Status status_1;
   MPI_Status status_2;


   for (i = 0; i < A_ARY; ++i)
   {
      if (((A_ARY * rank) + i + 1) < proc_num)
      {
         MPI_Recv(&((nodes + rank)->childnotready[i]), 1, MPI_INT, ((rank * A_ARY) + i + 1), 1, MPI_COMM_WORLD, &status_1);
      }
   }
   
   for (i = 0; i < A_ARY; i++)
   {
      (nodes + rank)->childnotready[i] = (nodes + rank)->havechild[i];
   }

   if (rank != 0)
   {
      buffer = 0;
      floor_val = floor((double)((rank - 1) / A_ARY));
      MPI_Send(&buffer, 1, MPI_INT, floor((double)((rank - 1) / A_ARY)), 1, MPI_COMM_WORLD);
   }
   
   if (rank != 0)
   {
      MPI_Recv(&((nodes + rank)->parentsense), 1, MPI_INT, ((rank - 1) / W_ARY), 2, MPI_COMM_WORLD, &status_2);

   }

   for (i = 0; i < W_ARY; i++)
   {
      if (((W_ARY * rank) + i + 1) < proc_num)
      {
         buffer2 = sense;
         MPI_Send(&buffer2, 1, MPI_INT, ((W_ARY * rank) + i + 1), 2, MPI_COMM_WORLD);
      }
   }
   
   sense = !sense;
}

void mcs_init(treenode *nodes, int proc_num, int i, int sense)
{
   int j;
   int floor_val;

   for (j = 0; j < A_ARY; j++)
   {
      if (((A_ARY * i) + j + 1) < proc_num)
      {
         (nodes + i)->havechild[j] = 1;
      }
      else
      {
         (nodes + i)->havechild[j] = 0;
      }
   }

   if (i == 0)
   {
      (nodes + i)->parentpointer = &((nodes + i)->dummy);
   }
   else
   {
      floor_val = floor((double)((i - 1) / A_ARY));
      (nodes + i)->parentpointer = &((nodes + floor_val)->childnotready[(i - 1) % A_ARY]);
   }

   for (j = 0; j < W_ARY; j++)
   {
      if (((W_ARY * i) + j + 1) < proc_num)
      {
         (nodes + i)->childpointers[j] = &(nodes + (W_ARY * i) + j + 1)->parentsense;
      }
      else
      {
         (nodes + i)->childpointers[j] = &((nodes + i)->dummy);
      }
   }

   for (j = 0; j < A_ARY; j++)
   {
      (nodes + i)->childnotready[i] = (nodes + i)->havechild[i];
   }

   (nodes + i)->parentsense = 0;
}

int main(int argc, char **argv)
{
   int BARRIER_NUM;
   int i;
   int rank;
   int proc_num;
   int sense;
   double start_time;
   double end_time;
   double time_diff;
   double global_start_time;
   double global_end_time;
   double global_time_diff;
   double total_time;
   double overall_total_time;
   double overall_avg_time;

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
   
   treenode nodes[proc_num];

   if (argc != 2)
   {
      printf("Error, invalid execution\nProper usage: mpirun -np <number of processes> ./mpi_mcs <number of barriers>\n");
      exit(-1);
   }
   else
   {
      BARRIER_NUM = atoi(argv[1]);
   }

   overall_avg_time = 0;
   overall_total_time = 0;

   mcs_init(nodes, proc_num, rank, sense); 

   for(i = 0; i < BARRIER_NUM; ++i)
   {
      sleep(10);
      if ((i % 2) == 0)
      {
         sense = 1;
      }
      else
      {
         sense = 0;
      }
      
      start_time = mysecond();
      printf("\nProcess %d in barrier %d\n", rank, i);
      mcs_barrier(nodes, proc_num, i, rank, sense);
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
