#!/bin/sh
#PBS -q cs6210 
#PBS -l nodes=12:sixcore
#PBS -l walltime=00:05:00
#PBS -N combined

for n1 in {2..8}
do
   for n2 in {2..12}
   do
	   OMPI_MCA_mpi_yield_when_idle=0 /opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile /nethome/pchoudhary32/CS6210_Project2/nodefile -np $n1 /nethome/pchoudhary32/CS6210_Project2/combined_without_printfs/combined 1000 $n2> /nethome/pchoudhary32/CS6210_Project2/combined_without_printfs/results/logc.txt
   done
done
# eof
