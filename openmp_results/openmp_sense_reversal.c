/* CS 6210 - Fall 2017
 * Sense-reversal Barrier
 * Palak Choudhary
 */

# include <omp.h>
# include <stdio.h>
# include <sys/time.h>
# include <stdlib.h>

typedef struct
{
   double avg_time_spent;
   double total_time_spent;
} Measures;

double mysecond()
{
   struct timeval tp;
   gettimeofday(&tp, NULL);
   return ((double) tp.tv_sec + (double) tp.tv_usec / 1.e6);
}

void sense_reversal_barrier(int THREAD_NUM, int *count, int *sense, int barrier, int local_sense)
{
   int thread;

   thread = omp_get_thread_num();
   local_sense = !local_sense;

   printf("Thread %d in barrier %d\n", thread, barrier);

#  pragma omp atomic
      (*count) -= 1;

   if (*count == 0)
   {
#     pragma omp critical
      {
         *count = THREAD_NUM;
         *sense = local_sense;
      }
   }

   else
      while (*sense != local_sense);

   printf("Thread %d out of barrier %d\n", thread, barrier);
}

Measures sense_reversal_init(int THREAD_NUM, int barrier, int *count, int *sense)
{
   int local_sense;
   double overall_start_time;
   double overall_end_time;
   Measures mes;

   if (barrier % 2 == 0)
      local_sense = 1;
   else
      local_sense = 0;

   omp_set_num_threads(THREAD_NUM);
   overall_start_time = mysecond();
#  pragma omp parallel shared(count, sense)
   {
      int thread;
      double start_time;
      double end_time;

      thread = omp_get_thread_num();
      start_time = mysecond(); 
      sense_reversal_barrier(THREAD_NUM, count, sense, barrier, local_sense);
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

int main(int argc, char ** argv)
{
   int BARRIER_NUM;
   int THREAD_NUM;
   int i;
   int count;
   int sense;
   Measures retval;
   Measures overall;

   if (argc != 3)
   {
      printf("Error, invalid number of arguments\nProper usage: ./openmp_sense_reversal <number of barriers> <number of threads>\n");
      exit(-1);
   }
   else
   {
      BARRIER_NUM = atoi(argv[1]);
      THREAD_NUM = atoi(argv[2]);
   }

   count = THREAD_NUM;
   sense = 1;
   overall.avg_time_spent = 0;
   overall.total_time_spent = 0;

   for (i = 0; i < BARRIER_NUM; ++i)
   {
      retval = sense_reversal_init(THREAD_NUM, i, &count, &sense);
      overall.avg_time_spent += retval.avg_time_spent;
      overall.total_time_spent += retval.total_time_spent;
   }

   overall.avg_time_spent /= BARRIER_NUM;
   overall.total_time_spent /= BARRIER_NUM;

   printf("Overall average time spent by a thread in a barrier: %f\n", overall.avg_time_spent);
   printf("Overall average time spent in a barrier: %f\n", overall.total_time_spent);

   return 0;
}
