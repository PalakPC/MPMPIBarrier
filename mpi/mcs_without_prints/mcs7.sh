#!/bin/sh
#PBS -q cs6210
#PBS -l walltime=00:05:00 
#PBS -l nodes=7:sixcore
#PBS -N mcs

#for n in {2..12}
#do
	OMPI_MCA_mpi_yield_when_idle=0 /opt/openmpi-1.4.3-gcc44/bin/mpirun -np 7 /nethome/pchoudhary32/CS6210_Project2/mpi/mcs_without_prints/mcs 1000 >> /nethome/pchoudhary32/CS6210_Project2/mpi/mcs_without_prints/results/logm.txt
#done
# eof
