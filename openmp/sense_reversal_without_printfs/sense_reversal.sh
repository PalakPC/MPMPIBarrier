#!/bin/sh
#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N sense_reversal

for n in {2..8}
do
	./sense_reversal 1000 $n > /nethome/pchoudhary32/CS6210_Project2/openmp/sense_reversal_without_printfs/results/logs.txt
	./sense_reversal 1000 $n 
done
#eof
