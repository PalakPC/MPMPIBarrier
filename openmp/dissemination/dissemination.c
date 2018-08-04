/* CS 6210 - Fall 2017
 * Project - 2
 * Dissemination Barrier
 * Palak Choudhary
 */

# include <stdio.h>
# include <stdbool.h>   //For boolean variables

# include "dissemination.h"

void dissemination_init(int NUM_THREADS, flags *allnodes)
{
   /*
    * Implementation of initialization part of Hensgen, Finkel, and Manber's dissemination barrier
    */

   int i;
   int j;
   int k;
   int r;
   int P;
   int logP;
   int temp;

   P = NUM_THREADS;
   logP = ceil(log2(P));

   for (i = 0; i < P; i++)
   {
      for (r = 0; r < 2; r++)
      {
         for (k = 0; k < logP; k++)
         {
            (allnodes + i)->myflags[r][k] = false;
            temp = fmod((i + pow(2, k)), P);
            for (j = 0; j < P; j++)
            {
               if (j == temp)
               {
                  (allnodes + i)->partnerflags[r][k] = &(allnodes + j)->myflags[r][k];
               }
            }
         }
      }
   }
}

void dissemination_barrier(int t, int NUM_THREADS, flags *allnodes, int barrier, bool *sense, int *parity)
{
   /*
    * Implementation of Hensgen, Finkel, and Manber's dissemination barrier
    */
   
   int instance;
   int logP;
   flags *localflags;

//   printf("Thread %d in barrier %d\n", t, barrier);

   localflags = allnodes;
   logP = ceil(log2(NUM_THREADS));

   for (instance = 0; instance < logP; instance++)
   {
      *localflags->partnerflags[*parity][instance] = *sense;
      while (localflags->myflags[*parity][instance] != *sense);   //Wait till partner is done
   }

   if (*parity == 1)
      *sense = !(*sense);

   *parity = 1 - *parity;

//   printf("Thread %d out of barrier %d\n", t, barrier);
}
