#PBS -q cs6210
#PBS -l nodes=1:fourcore
#PBS -l walltime=00:05:00
#PBS -N openmp_sense_reversal
#PBS -o sense_reversal_results

pwd; hostname; date

for n in {2..8}
do
	echo "Thread count: $(n)"
	echo "-------------------------------------------------------------------------------------------------------"
	./sense_reversal 1000 $(n) 
	echo "-------------------------------------------------------------------------------------------------------"
done

date
