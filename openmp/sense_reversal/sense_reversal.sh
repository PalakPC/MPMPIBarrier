#!/bin/sh
#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N sense_reversal

for n in {2..8}
do
#touch /nethome/pchoudhary32/CS6210_Project2/opemnp/sense_reversal/results/logs_$n.txt
	/nethome/pchoudhary32/CS6210_Project2/openmp/sense_reversal/sense_reversal 1000 $n > /nethome/pchoudhary32/CS6210_Project2/openmp/sense_reversal/results/logs_$n.txt
done
#eof
