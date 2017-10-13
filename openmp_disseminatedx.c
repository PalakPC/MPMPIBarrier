/* CS 6210 - Fall 2017
 * Dissemination Barrier
 * Palak Choudhary
 */

# include <omp.h>
# include <stdio.h>
# include <sys/time.h>
# include <time.h>
# include <stdlib.h>
# include <math.h>
# include <unistd.h>

# define MAX_LOGP 3  //Maximum number of threads is 8, so maximum log2(P) will be 3
# define MAX_P 8

int P;

typedef struct
{
   int myflags[2][MAX_LOGP];
   int *partnerflags[2][MAX_LOGP];
} flags;

flags allnodes[MAX_P];

double mysecond()
{
   struct timeval tp;
   gettimeofday(&tp, NULL);
   return ((double) tp.tv_sec + (double) tp.tv_usec / 1.e6);
}

void dissemination_barrier()
{
   int t = omp_get_thread_num();
   printf("Thread %d in dissemination at time %f\n", t, mysecond());
   int parity = 0;
   int instance;
   int logP = ceil(log2(P));
   int sense = 1;
   flags *localflags;
#  pragma omp critical
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
   printf("Thread %d out of dissemination at time %f\n", t, mysecond());
}

void doWork () {
   int waitSecs = rand() % 10 + 1;
   sleep (waitSecs);
}

int main(int argc, char **argv)
{
   int i;
   int j;
   int k;
   int r;
   int temp;
   if (argc != 2)
   {
      printf("Error, no argument");
      exit(-1);
   }
   else
      P = atoi(argv[1]);

   int logP = ceil(log2(P));
   srand (time(NULL));
   double total_time_start;
   double total_time_end;
   omp_set_num_threads(P);
   total_time_start = mysecond();
   printf("Execution time start %f\n", total_time_start);
   for (r = 0; r < 2; r++)
   {
      for (k = 0; k < logP; k++)
      {
         for (i = 0; i< P; i++)
         {
            //#              pragma omp single
            allnodes[i].myflags[r][k] = 0;
            temp = fmod((i + pow(2, k)), P);
            for (j = 0; j <P; ++j)
            {
               if (j == temp)
               {
                  //#                 pragma omp critical
                  allnodes[i].partnerflags[r][k] = &allnodes[j].myflags[r][k];
               }
            }
         }
      }
   }
#  pragma omp parallel shared(allnodes) //Initialization
   {
      doWork();
      /* int i;
         int j;
         int k;
         int r;
         int temp;
         int logP = ceil(log2(P));
       */int t = omp_get_thread_num();

      printf("Thread %d in finished work at time %f\n", t, mysecond());
      //   }
      //   total_time_end = mysecond();
      //   printf("Initialization ended at %f\n", total_time_end);
      //   total_time_start = mysecond();
      //   printf("Execution time start %f\n", total_time_start);
      //#  pragma omp parallel shared(allnodes)
      //   {
      printf ("Thread id %d calling barrier\n", t);
      dissemination_barrier();
}
total_time_end = mysecond();
printf("Execution ended at %f\n", total_time_end);
printf("Execution time: %f seconds\n", total_time_end - total_time_start);
return 0;
}
