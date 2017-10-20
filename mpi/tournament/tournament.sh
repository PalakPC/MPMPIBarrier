PBS -q cs6210
#PBS -l walltime=00:05:00
#PBS -N mpiresult
for n in {2..12}
do
  	#PBS -l nodes=$n:sixcore
        OMPI_MCA_mpi_yield_when_idle=0 mpirun -np $n /nethome/gjoshi30/CS6210_P$
done
# eof

