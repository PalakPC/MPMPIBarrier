/* CS 6210 - Fall 2017
 * Project - 2
 * MCS Barrier
 * Palak Choudhary
 */

# include <stdio.h>
# include <stdbool.h>

# include "mcs.h"

void mcs_init(treenode *nodes, int NUM_PROCESSES, int i, bool *sense)
{
   int j;
   int floor_val;

   *sense = true;

   for (j = 0; j < A_ARY; j++)
   {
      if (((A_ARY * i) + j + 1) < NUM_PROCESSES)
      {
         (nodes + i)->havechild[j] = true;
      }
      else
      {
         (nodes + i)->havechild[j] = false;
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
      if (((W_ARY * i) + j + 1) < NUM_PROCESSES)
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

   (nodes + i)->parentsense = false;
}

void mcs_barrier(treenode *nodes, int NUM_PROCESSES, int barrier, int rank, bool *sense)
{
   int i;
   int floor_val;
   bool buffer;
   bool buffer2;
   MPI_Status status_1;
   MPI_Status status_2;

//   printf("\nProcess %d in barrier %d\n", rank, barrier);
   
   for (i = 0; i < A_ARY; ++i)
   {
      if (((A_ARY * rank) + i + 1) < NUM_PROCESSES)
      {
         MPI_Recv(&((nodes + rank)->childnotready[i]), 1, MPI_BYTE, ((rank * A_ARY) + i + 1), 1, MPI_COMM_WORLD, &status_1);
      }
   }
   
   for (i = 0; i < A_ARY; i++)
   {
      (nodes + rank)->childnotready[i] = (nodes + rank)->havechild[i];
   }

   if (rank != 0)
   {
      buffer = false;
      floor_val = floor((double)((rank - 1) / A_ARY));
      MPI_Send(&buffer, 1, MPI_BYTE, floor_val, 1, MPI_COMM_WORLD);
   }
   
   if (rank != 0)
   {
      MPI_Recv(&((nodes + rank)->parentsense), 1, MPI_BYTE, ((rank - 1) / W_ARY), 2, MPI_COMM_WORLD, &status_2);

   }

   for (i = 0; i < W_ARY; i++)
   {
      if (((W_ARY * rank) + i + 1) < NUM_PROCESSES)
      {
         buffer2 = sense;
         MPI_Send(&buffer2, 1, MPI_BYTE, ((W_ARY * rank) + i + 1), 2, MPI_COMM_WORLD);
      }
   }
   
   *sense = !(*sense);
//   printf("Process %d out of barrier %d\n", rank, barrier);
}
