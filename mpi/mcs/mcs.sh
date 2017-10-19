#!/bin/sh
#PBS -q cs6210
#PBS -l walltime=00:05:00 
#PBS -N mcs

for n in {2..12}
do
   #PBS -l nodes=$n:sixcore
   OMPI_MCA_mpi_yield_when_idle=0 /opt/openmpi-1.4.3-gcc44/bin/mpirun -np $n /nethome/pchoudhary32/CS6210_Project2/mpi/mcs/mcs 1000 > /nethome/pchoudhary32/CS6210_Project2/mpi/mcs/results/logm_$n.txt
done
#eof
