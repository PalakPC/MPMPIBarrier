#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <mpi.h>
#include <math.h>
#include <sys/time.h>

void tour_barrier( int rank, int numprocs, int max_rnd);

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
	
    	int max_round = ceil(log(numberofprocess) / log(2));

	int j;
	double start_time, end_time, avg;
	printf("process %d is working\n", rank);
	avg = 0.0;
	for (j=0; j<iterations; j++) 
	{
		avg = 0.0;
		//start_time = mysecond();
		printf("Entering tournament barrier mpi for barrier :%d by process no %d\n", j, rank);		
		tour_barrier(rank, numberofprocess, max_round);
		printf("Exiting tournament barrier mpi for barrier :%d by process no %d\n", j, rank);
		//end_time = mysecond();
		//printf("time for barrier no %d for process rank %d is %f\n", j, rank, (end_time - start_time));
		avg = avg + (end_time - start_time);
	}

	printf("The average time for %d barriers is %f\n", iterations, avg);
	
        MPI_Finalize();
	return 0;
}
