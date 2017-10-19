#!/bin/sh
#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N baseline

for n in {2..8}
do
#touch /nethome/pchoudhary32/CS6210_Project2/opemnp/dissemination/results/logd_$n.txt
	/nethome/pchoudhary32/CS6210_Project2/openmp/baseline/baseline 1000 $n > /nethome/pchoudhary32/CS6210_Project2/openmp/baseline/results/logb_$n.txt
done
#eof
