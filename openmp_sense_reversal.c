# include <omp.h>
# include <stdio.h>
# include <sys/time.h>
# include <stdlib.h>

int P;
int count;
int sense;

double mysecond()
{
   struct timeval tp;
   gettimeofday(&tp, NULL);
   return ((double) tp.tv_sec + (double) tp.tv_usec / 1.e6);
}

void sense_reversal_barrier()
{
   int local_sense = 0;
   int flag = 0;
#  pragma omp critical
   {
      count--;
      if (count == 1)
      {
         flag = 1;
         count = P;
         sense = local_sense;
      }
   }
   if (flag == 0)
      while(sense != local_sense);
}


int main(int argc, char ** argv)
{
   if (argc != 2)
   {
      printf("Error, invalid number of arguments");
      exit(-1);
   }
   else
      P = atoi(argv[1]);

   count = P;
   sense = 1;
   omp_set_num_threads(P);
#  pragma omp parallel shared(count, sense)
   {
      int i = omp_get_thread_num();
      printf("Thread %d in sense_reversal at time %f\n", i, mysecond());
      sense_reversal_barrier();
      printf("Thread %d out of sense_reversal at time %f\n", i, mysecond());
   }
   return 0;
}

