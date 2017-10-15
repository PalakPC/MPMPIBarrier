
/* CS 6210 - Fall 2017
 * Combined Barrier
 * Palak Choudhary
 */

# include <mpi.h>
# include <omp.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <math.h>
# include <unistd.h>

# define W_ARY 2
# define A_ARY 4
# define MAX_LOGP 3  //Maximum number of threads is 8, so maximum log2(P) will be 3
# define MAX_P 8

typedef struct
{
   double avg_time_spent;
   double total_time_spent;
} Measures;
typedef struct
{
   int myflags[2][MAX_LOGP];
   int *partnerflags[2][MAX_LOGP];
} flags;

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
void dissemination_barrier(int logP, flags *allnodes, int barrier, int rank)
{
   int t;
   int parity;
   int instance;
   int sense;
   flags *localflags;

   t = omp_get_thread_num();
   parity = 0;
   sense = 1;

   printf("Thread %d of process %d in barrier %d\n", t, rank, barrier);

#  pragma omp critical
   {
      localflags = allnodes + t;
   }

   for (instance = 0; instance < logP; instance++)
   {
#     pragma omp critical
      {
         *localflags->partnerflags[parity][instance] = sense;
      }
      while (localflags->myflags[parity][instance] != sense);
   }

   if (parity == 1)
      sense = !sense;

   parity = 1 - parity;

   printf("Thread %d of process %d out of barrier %d\n", t, rank, barrier);
}

Measures dissemination_init(int THREAD_NUM, int barrier, int rank)
{
   int i;
   int j;
   int k;
   int r;
   int P;
   int temp;
   int logP;
   double overall_start_time;
   double overall_end_time;
   flags allnodes[MAX_P];
   Measures mes;

   P = THREAD_NUM;
   logP = ceil(log2(P));

   for (r = 0; r < 2; r++)
   {
      for (k = 0; k < logP; k++)
      {
         for (i = 0; i < P; i++)
         {
            allnodes[i].myflags[r][k] = 0;
            temp = fmod((i + pow(2, k)), P);
            for (j = 0; j <P; ++j)
            {
               if (j == temp)
               {
                  allnodes[i].partnerflags[r][k] = &allnodes[j].myflags[r][k];
               }
            }
         }
      }
   }

   omp_set_num_threads(P);
   overall_start_time = mysecond();
#  pragma omp parallel shared(allnodes) //Initialization
   {
      int thread;
      double start_time;
      double end_time;

      thread = omp_get_thread_num();
      start_time = mysecond();
      dissemination_barrier(logP, allnodes, barrier, rank);
      end_time = mysecond();
      printf("Thread %d spent %f in barrier %d\n", thread, (end_time - start_time), barrier);
      mes.avg_time_spent = mes.avg_time_spent + (end_time - start_time);
   }

   overall_end_time = mysecond();
   mes.total_time_spent = overall_end_time - overall_start_time;
   mes.avg_time_spent = mes.avg_time_spent / THREAD_NUM;

   printf("\nTotal time spent in barrier %d: %f\n", barrier, mes.total_time_spent);
   printf("Average time spent by a thread in barrier %d: %f\n\n", barrier, mes.avg_time_spent);

   return mes;
}

int main(int argc, char **argv)
{
   int BARRIER_NUM;
   int THREAD_NUM;
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

   if (argc != 3)
   {
      printf("Error, invalid execution\nProper usage: mpirun -np <number of processes> ./combined <number of barriers> <number of threads>\n");
      exit(-1);
   }
   else
   {
      BARRIER_NUM = atoi(argv[1]);
      THREAD_NUM = atoi(argv[2]);
   }

   overall_avg_time = 0;
   overall_total_time = 0;

   mcs_init(nodes, proc_num, rank, sense); 

   for(i = 0; i < BARRIER_NUM; ++i)
   {
      sleep(1);
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
      dissemination_init(THREAD_NUM, i, rank);
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
