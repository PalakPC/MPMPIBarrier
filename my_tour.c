#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

void tournament_barrier( int rank, int numprocs, int max_rnd) 
	{
	    int tag = 1;
	    int data = 1;
	    int self = 0;
	    int curr_rnd;
	    
	    for(curr_rnd = 1; curr_rnd <= max_rnd; curr_rnd ++)
		{
	 	       if(rank % (int)pow(2, curr_rnd) == 0)
			{
	        	    if(rank + pow(2, curr_rnd - 1) < numprocs)
			    {
	        	        MPI_Recv( &self, 1, MPI_INT, rank + pow(2, curr_rnd - 1), tag, MPI_COMM_WORLD, NULL);
	        	    }
	        	}
	        	else 
			{
	        	    MPI_Send( &data, 1, MPI_INT, rank - pow(2, curr_rnd - 1), tag, MPI_COMM_WORLD);
	        	    MPI_Recv( &self, 1, MPI_INT, rank - pow(2, curr_rnd - 1), tag, MPI_COMM_WORLD, NULL);
	        	    break;
	        	}
	    	}	

    		for(curr_rnd = max_rnd; curr_rnd >= 1; curr_rnd --)
		{	
	        	if(rank % (int)pow(2, curr_rnd) == 0)
			{
	        	    if(rank + pow(2, curr_rnd - 1) < numprocs)
			    {
	        	        MPI_Send( &data, 1, MPI_INT, rank + pow(2, curr_rnd - 1), tag, MPI_COMM_WORLD);
	        	    }
       			}
		}
}
