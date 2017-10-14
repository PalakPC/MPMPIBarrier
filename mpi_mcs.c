# include <math.h>
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <mpi.h>
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
   sleep(10);
   int i;
   int buffer;
   int buffer2;
   int floor_val;
   MPI_Status status_1;
   MPI_Status status_2;

   printf("Process %d in barrier %d\n", rank, barrier);

   for (i = 0; i < A_ARY; ++i)
   {
      if (((A_ARY * rank) + i + 1) < proc_num)
      {
         printf("%d has childnotready %d as %d\n", rank, i, (nodes + rank)->childnotready[i]); 
         MPI_Recv(&((nodes + rank)->childnotready[i]), 1, MPI_INT, ((rank * A_ARY) + i + 1), 1, MPI_COMM_WORLD, &status_1);
         printf("%d got childnotready %d as %d from %d\n", rank, i, ((nodes + rank)->childnotready[i]), ((rank * A_ARY) + i + 1));
      }
   }
   
   for (i = 0; i < A_ARY; i++)
   {
      (nodes + rank)->childnotready[i] = (nodes + rank)->havechild[i];
   }

   if (rank != 0)
   {
      //*((nodes + rank)->parentpointer) = 0;
      buffer = 0;
      floor_val = floor((double)((rank - 1) / A_ARY));
      MPI_Send(&buffer, 1, MPI_INT, floor((double)((rank - 1) / A_ARY)), 1, MPI_COMM_WORLD);
      printf("%d sent parentpointer %d to %d\n", rank, 0, floor_val);
   }
   
   if (rank != 0)
   {
      printf("%d has parentsense as %d\n", rank, (nodes + rank)->parentsense); 
      MPI_Recv(&((nodes + rank)->parentsense), 1, MPI_INT, ((rank - 1) / W_ARY), 2, MPI_COMM_WORLD, &status_2);
      printf("%d got parentsense %d from %d\n", rank, ((nodes + rank)->parentsense), ((rank - 1) / W_ARY));

   }

   for (i = 0; i < W_ARY; i++)
   {
      if (((W_ARY * rank) + i + 1) < proc_num)
      {
         buffer2 = sense;
         MPI_Send(&buffer2, 1, MPI_INT, ((W_ARY * rank) + i + 1), 2, MPI_COMM_WORLD);
         printf("%d sent childpointer %d as %d to %d\n", rank, i, sense, ((W_ARY * rank) + i + 1));
      }
   }
   
   sense = !sense;
   printf("Process %d out of barrier %d\n", rank, barrier);
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

//   mcs_barrier(nodes, proc_num, barrier, i, sense);
}

int main(int argc, char **argv)
{
   int BARRIER_NUM;
   int i;
   int rank;
   int proc_num;
   int sense;

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

   mcs_init(nodes, proc_num, rank, sense); 

   for(i = 0; i < BARRIER_NUM; ++i)
   {
      if ((i % 2) == 0)
      {
         sense = 1;
      }
      else
      {
         sense = 0;
      }
      mcs_barrier(nodes, proc_num, i, rank, sense);
   }

   MPI_Finalize();
   return 0;
}
