#PBS -q cs6210
#PBS -l nodes=1:fourcore
#PBS -l walltime=00:05:00
#PBS -N ompresults
for n in {2..12}
do
	./openmp_dissemination 100 $n 
done
#eof
