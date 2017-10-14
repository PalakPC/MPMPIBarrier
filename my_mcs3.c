#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

typedef struct treenode
{
	int childnotready[4]; 
	bool havechild[4];
	int arrivaltreeparent;
	int wakeuptreeparent;
	int childpointer[2];
} mcsnode;

static mcsnode *node;

void mcs_init(int n_proc, int rank) 
{
	node = (mcsnode *) malloc(sizeof(mcsnode));
	int i;
	for (i=0; i<4; i++) 
	{
		if (4*rank+i+1<n_proc) 
		{
			node->havechild[i]=1;
			node->childnotready[i] = 4*rank+i+1;
		}
		else 
		{
			node->havechild[i]=0;
			node->childnotready[i]=-1;
		}

	}
	if (rank!=0) 
	{
		node->arrivaltreeparent = (rank-1)/4;
		node->wakeuptreeparent = (rank-1)/2;
	}
	else 
	{
		node->arrivaltreeparent = -1;
		node->wakeuptreeparent = -1;
	}
	for (i=0; i<2; i++)	
	{
		if (2*rank+i+1<n_proc)
			node->childpointer[i] = 2*rank+i+1;
		else
			node->childpointer[i] = -1;
	}

}

void mcs_barrier(int n_proc, int rank) 
{
	int i, tag=1;
	unsigned char data=1;
	MPI_Status status;
	for (i=0; i<4; i++) 
	{
		if (node->havechild[i]==1)
		{
			MPI_Recv(&data, 1, MPI_BYTE, node->childnotready[i], tag, MPI_COMM_WORLD, &status);
			
		}
		
	}

	if (rank!=0) 
	{
		//printf("process %d arrived at the barrier\n", rank);		
		MPI_Send(&data, 1, MPI_BYTE, node->arrivaltreeparent, tag, MPI_COMM_WORLD);
		MPI_Recv(&data, 1, MPI_BYTE, node->wakeuptreeparent, tag, MPI_COMM_WORLD, &status); 
		//printf("process %d exited the barrier\n", rank);
	}
	
	for (i=0; i<2; i++)	
	{
		if (node->childpointer[i]!=-1)
		{
			MPI_Send(&data, 1, MPI_BYTE, node->childpointer[i], tag, MPI_COMM_WORLD);
			
		}
		
	}
}

void finalize() 
{

	free(node);
	
}
