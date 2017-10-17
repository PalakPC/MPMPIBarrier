#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <mpi.h>
#include <sys/time.h>


void mcs_init(int num_threads, int rank);
void mcs_barrier(int num_threads, int rank);
void finalize();


double mysecond()
{
   struct timeval tp;
   gettimeofday(&tp, NULL);
   return ((double) tp.tv_sec + (double) tp.tv_usec / 1.e6);
}

int main(int argc, char **argv)
{
	int iterations, rank;
	int numberofprocess;
	MPI_Init(&argc, &argv);
	if (argc >=2) 
	{
		iterations = atoi(argv[1]);
	}
	else 
	{
		printf("Please specify number of iterations");
		return 0;
	}

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numberofprocess);

	mcs_init(numberofprocess, rank);
	
	int j, i=0;
	double start_time, end_time, avg;
	printf("process %d is working\n", rank);
	avg = 0.0;		
	start_time = mysecond();
	for (j=0; j<iterations; j++) 
	{
		//avg = 0.0;
		//start_time = mysecond();
		printf("Entering mcs for barrier:%d by process no %d\n", j, rank);
		mcs_barrier(numberofprocess,rank);
		printf("Exiting mcs for barrier:%d by process no %d\n", j, rank);
		//end_time = mysecond();
		//printf("time for barrier no %d for process rank %d is %f\n", j, rank, (end_time - start_time));
		avg = avg + (end_time - start_time);
	}
	end_time = mysecond();
	avg = avg + (end_time - start_time);
	//printf("time for barrier no %d for process rank %d is %f\n", j, rank, (end_time - start_time));	
	printf("The average time for %d process for %d barriers is %f\n", rank,iterations, avg);

	finalize();
        MPI_Finalize();
	return 0;
}
