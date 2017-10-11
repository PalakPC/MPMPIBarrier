/* CS 6210 - Fall 2017
 * Dissemination Barrier
 * Palak Choudhary
 */

# include <omp.h>
# include <stdio.h>
# include <sys/time.h>
# include <stdlib.h>
# include <math.h>

# ifndef P
#  define P 2
# endif

# define MAX_LOGP 8  //Maximum number of threads is 8, so maximum log(P) will be 3


typedef struct
{
   int myflags[2][MAX_LOGP];
   int *partnerflags[2][MAX_LOGP];
} flags;

flags allnodes[P];

double mysecond()
{
   struct timeval tp;
   gettimeofday(&tp, NULL);
   return ((double) tp.tv_sec + (double) tp.tv_usec / 1.e6);
}

void dissemination_barrier()
{
   int parity = 0,
       instance;
   int logP = ceil(log2(P));
   int sense = 1;
   flags *localflags;
   int t = omp_get_thread_num();
//#  pragma omp critical
   localflags = &allnodes[t];
   for (instance = 0; instance < logP; instance++)
   {
#     pragma omp critical
      *localflags->partnerflags[parity][instance] = sense;
      while (localflags->myflags[parity][instance] != sense);
   }
   if (parity == 1)
      sense = !sense;
   parity = 1 - parity;
}

int main()
{
   double total_time_start,
          total_time_end;
   omp_set_num_threads(P);
#  pragma omp parallel shared(allnodes) //Initialization
   {
      int i,
          j,
          k,
          r;
      int temp, x;
      int logP = ceil(log2(P));
      i = omp_get_thread_num();
      printf("Thread %d\n", i);
      for (r = 0; r < 2; r++)
      {
         for (k = 0; k < logP; k++)
         {
#           pragma omp critical
            allnodes[i].myflags[j][k] = 0;
            temp = fmod((i + pow(2, k)), P);
#           pragma omp critical
            for (j = 0; j < P; j++)
            {
               if (j == temp)
	            {
                  allnodes[i].partnerflags[r][k] = &allnodes[j].myflags[r][k];
                  x = j;
	            }
            }
         }
      }
   }
   total_time_start = mysecond();
#  pragma omp parallel shared(allnodes)
   {
     int t = omp_get_thread_num();
   printf("Thread %d in dissemination at time %f\n", t, mysecond());
      dissemination_barrier();

   printf("Thread %d out of dissemination at time %f\n", t, mysecond());
   }
   total_time_end = mysecond();
   printf("Execution time: %f seconds\n", total_time_end - total_time_start);
   return 0;
}
