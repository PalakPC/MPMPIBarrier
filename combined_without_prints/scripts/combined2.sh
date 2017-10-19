#!/bin/sh
#PBS -q cs6210 
#PBS -l walltime=00:30:00
#PBS -N combined

#PBS -l nodes=2:sixcore
for n2 in {2..12}
do
	OMPI_MCA_mpi_yield_when_idle=0 mpirun -np 2 /nethome/gjoshi30/CS6210_Project2/combined_without_prints/combined 1000 $n2 >> /nethome/gjoshi30/CS6210_Project2/combined_without_prints/results/logc2.txt
done
#eof
