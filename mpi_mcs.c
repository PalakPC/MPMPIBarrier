# include <stdio.h>
# include <stdlib.h>
# include <mpi.h>

# define W_ARY 2
# define A_ARY 4

typedef struct
{
   int parentsense;
   int *parentpointer;
   int childpointers[W_ARY];
   int havechild[A_ARY];
   int childnotready[A_ARY];
   int dummy;
} treenode;

void mpi_init(treenode *nodes, int proc_num, int barrier, int i)
{
   int vpid;
   int sense;
   int j;

   sense = 1;
   
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

   (nodes + i)->parentpointer = nodes + floor((i - 1) / A_ARY);




int main(int argc, char **argv)
{
   int BARRIER_NUM;
   int i;
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

   for(i = 0; i < BARRIER_NUM; ++i)
   {
      mpi_init(nodes, proc_num, i, rank);
   }

   return 0;
}
