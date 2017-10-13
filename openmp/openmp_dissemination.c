/* CS 6210 - Fall 2017
 * Dissemination Barrier
 * Palak Choudhary
 */

# include <omp.h>
# include <stdio.h>
# include <sys/time.h>
# include <stdlib.h>
# include <math.h>

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

double mysecond()
{
   struct timeval tp;
   gettimeofday(&tp, NULL);
   return ((double) tp.tv_sec + (double) tp.tv_usec / 1.e6);
}

void dissemination_barrier(int logP, flags *allnodes, int barrier)
{
   int t;
   int parity;
   int instance;
   int sense;
   flags *localflags;

   t = omp_get_thread_num();
   parity = 0;
   sense = 1;

   printf("Thread %d in barrier %d\n", t, barrier);

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

   printf("Thread %d out of barrier %d\n", t, barrier);
}

Measures dissemination_init(int THREAD_NUM, int barrier)
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
      dissemination_barrier(logP, allnodes, barrier);
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
   Measures retval;
   Measures overall;

   if (argc != 3)
   {
      printf("Error, invalid number of arguments\nProper usage: ./openmp_dissemination <number of barriers> <number of threads>\n");
      exit(-1);
   }
   else
   {
      BARRIER_NUM = atoi(argv[1]);
      THREAD_NUM = atoi(argv[2]);
   }

   overall.avg_time_spent = 0;
   overall.total_time_spent = 0;

   for (i = 0; i < BARRIER_NUM; i++)
   {
      retval = dissemination_init(THREAD_NUM, i);
      overall.avg_time_spent += retval.avg_time_spent;
      overall.total_time_spent += retval.total_time_spent;
   }

   overall.avg_time_spent /= BARRIER_NUM;
   overall.total_time_spent /= BARRIER_NUM;

   printf("Overall average time spent by a thread in a barrier: %f\n", overall.avg_time_spent);
   printf("Overall average time spent in a barrier: %f\n", overall.total_time_spent);

   return 0;
}
