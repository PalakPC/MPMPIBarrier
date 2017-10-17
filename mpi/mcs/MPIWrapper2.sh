#PBS -q cs6210
#PBS -l nodes=12:sixcore
#PBS -l walltime=00:05:00 
#PBS -N mpi 
OMPI_MCA_mpi_yield_when_idle=0 /opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 12 mcs 4
