#!/bin/sh
#PBS -q cs6210 
#PBS -l walltime=00:07:00
#PBS -N combined

#PBS -l nodes=8:sixcore
for n2 in {11..12}
do
	OMPI_MCA_mpi_yield_when_idle=0 /opt/openmpi-1.4.3-gcc44/bin/mpirun -np $(($n2 * 8)) /nethome/pchoudhary32/CS6210_Project2/mpi/mcs_without_prints/mcs 1000 >> /nethome/pchoudhary32/CS6210_Project2/mpi/mcs_without_prints/results/logc.txt
done
#eof
