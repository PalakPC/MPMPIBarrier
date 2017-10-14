
#PBS -q cs6210 
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N MPIResults

pushd /home/AOS/Project2/MPI
# Run the program
echo "*** STARTED: `date` on `hostname` ***"
for n1 in {2..12}; do
	echo "Num of processes = $n1"	
	OMPI_MCA_mpi_yield_when_idle=0 /opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np $n1 /home/AOS/Project2/MPI/tournament 1000000 > logt$n1.txt
	echo "---------------------------------------------------------------------"
done

for n2 in {2..12}; do
	echo "Num of processes = $n"	
	OMPI_MCA_mpi_yield_when_idle=0 /opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np $n2 /home/AOS/Project2/MPI/mcs 1000000 > logm$n2.txt
	echo "---------------------------------------------------------------------"
done

echo "*** COMPLETED: `date` on `hostname` ***"
popd
# eof

