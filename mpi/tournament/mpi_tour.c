
# include <mpi.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <math.h>
# include <unistd.h>

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
enum possible_roles
{
	WINNER,
	LOSER,
	BYE,
	CHAMPION,
	DROPOUT
};

typedef struct 
{
	enum possible_roles curr_role; 
	int opponent;
}round_node;

void tour_barrier(round_node* curr_proc_node, int rank, int n_rnds) 
{	int round = 1;
	int exit =0;
	MPI_Status status;
	int tag = 1;
	int msg;
	if(n_rnds > 0)
	{
		while(1)
		{
			round_node* curr_node = curr_proc_node + round;
			//printf("process rank : %d, round no : %d, role : %d\n\n", rank, round, curr_node->curr_role);
			exit = 0;
			switch(curr_node->curr_role)
			{
				case LOSER : 
				msg = round;
				MPI_Send(&msg, 1, MPI_INT, curr_node->opponent, tag, MPI_COMM_WORLD);
				MPI_Recv(&msg, 1, MPI_INT, curr_node->opponent, tag, MPI_COMM_WORLD, &status);
				exit = 1; break;
				case WINNER :
				MPI_Recv(&msg, 1, MPI_INT, curr_node->opponent, tag, MPI_COMM_WORLD, &status); break;
				case CHAMPION : 
				MPI_Recv(&msg, 1, MPI_INT, curr_node->opponent, tag, MPI_COMM_WORLD, &status);
				msg = round;
				MPI_Send(&msg, 1, MPI_INT, curr_node->opponent, tag, MPI_COMM_WORLD);
				exit = 1; break;
				case BYE : break;
				case DROPOUT : break;
			}
			if(exit == 1) break;
			round ++;
		}
		 //printf("Process : %d exited\n\n", rank); 
		while(1)
		{
			round --; 
			round_node* curr_node = curr_proc_node + round; 
			exit = 0;	
			switch(curr_node->curr_role)
			{
				case LOSER : break;
				case WINNER : 
				msg = round; 
				MPI_Send(&msg, 1, MPI_INT, curr_node->opponent, tag, MPI_COMM_WORLD); break;
				case BYE : break;
				case CHAMPION : break;
				case DROPOUT : exit = 1 ; break;
			}
			if(exit == 1) break;
		}
	}
}
void tournament_init(round_node* curr_node_rnd, int rank, int proc_num)
{
	int state;
	int i;
	if(proc_num > 0)
	{
		unsigned int max_rnds = ceil(log(proc_num)/log(2));
		if(curr_node_rnd)
		{
			for(i=0; i<=max_rnds; i++)
			{
				round_node* curr_rnd = curr_node_rnd + i;
				//printf("current proc = %d, round = %d\n", rank, i);
				state = 0;
				if(i > 0)
				{
					if((rank % (int)pow(2,i)) == 0)
					{
						if(((rank + (int)pow(2, i-1)) < proc_num) && ((int)pow(2,i) < proc_num))
						{
							curr_rnd->curr_role = WINNER;
							state = 1;
							//printf("current proc = %d round = %d : WINNER\n", rank, i);
						}
						else if((rank + (int)pow(2, i-1)) >= proc_num)
						{
							state = 1;
							curr_rnd->curr_role = BYE;
							//printf("current proc = %d round = %d : BYE\n", rank, i);
						}
					}
					if(state == 0)
					{
						if((rank % (int)pow(2,i)) == (int)pow(2,i-1))
						{
							curr_rnd->curr_role = LOSER;
							//printf("current proc = %d round = %d : LOSER\n", rank, i);
						}
						else if((rank == 0) && ((int)pow(2,i) >= proc_num))
						{
							curr_rnd->curr_role = CHAMPION;
							//printf("current proc = %d round = %d : CHAMPION\n", rank, i);
						}
					}
				}
				else if (i == 0)
				{
					curr_rnd->curr_role = DROPOUT;
					//printf("current proc = %d round = %d : DROPOUT\n", rank, i);
				}
				int flag = -1;

				switch(curr_rnd->curr_role)
				{
					case LOSER : flag = rank - (int)pow(2, i-1); break;
					case WINNER :
					case CHAMPION : flag = rank + (int)pow(2, i-1); break;
				}
				if(flag>=0) curr_rnd->opponent = flag;
			}
		}
	}
}

int main(int argc, char **argv)
{
	int n_barriers; 
	int max_rnds;
        int rank;
        int proc_num;
        int sense;
   	double start_time;
   	double end_time;
   	double time_diff;
   	double global_start_time;
   	double global_end_time;
   	double global_time_diff;
   	double total_time;
   	double overall_total_time;
   	double overall_avg_time;

	MPI_Init(&argc, &argv);
   	MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
   	MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
	
	round_node* curr_proc_rnd; 
	max_rnds = ceil(log(proc_num) / log(2));
	
	curr_proc_rnd = (round_node*) malloc(sizeof( round_node)*(max_rnds + 1)); 
	
	if (argc != 2)
   	{
      		printf("Error, invalid execution\nProper usage: mpirun -np <number of processes> ./mpi_mcs <number of barriers>\n");
      		exit(-1);
   	}
   	else
   	{
      		n_barriers = atoi(argv[1]);
   	}

   	overall_avg_time = 0;
   	overall_total_time = 0;
	tournament_init(curr_proc_rnd, rank, proc_num);
	
	for(int i=0; i < n_barriers; i++)
	{
	
		start_time = mysecond();
	        printf("\nProcess %d in barrier %d\n", rank, i);
	        tour_barrier(curr_proc_rnd, rank, max_rnds);
	        printf("Process %d out of barrier %d\n", rank, i);
	        end_time = mysecond();
	
		time_diff = end_time - start_time;
	        printf("Process %d spent %f in barrier %d\n", rank, time_diff, i);

		MPI_Reduce(&start_time, &global_start_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
     	 	MPI_Reduce(&end_time, &global_end_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
      		MPI_Reduce(&time_diff, &global_time_diff, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	
		if (rank == 0)
     		 {
         		total_time = global_end_time - global_start_time;
		        overall_total_time += total_time;
        		printf("\nTotal time spent in barrier %d: %f\n", i, total_time);
      			global_time_diff /= proc_num;
         		overall_avg_time += global_time_diff;
         		printf("Average time spent by a thread in barrier %d: %f\n\n", i, global_time_diff);
      		 }

	}
	if (rank == 0)
	{
		overall_avg_time /= n_barriers;
	        overall_total_time /= n_barriers;

      		printf("Overall average time spent by a process in a barrier: %f\n", overall_avg_time);
      		printf("Overall average time spent in a barrier: %f\n", overall_total_time);
	}
	MPI_Finalize();
	return 0;
}
