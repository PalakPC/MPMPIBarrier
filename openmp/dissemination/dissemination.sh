#!/bin/sh
#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N dissemination

for n in {2..8}
do
	/nethome/pchoudhary32/CS6210_Project2/openmp/dissemination/dissemination 1000 $n > /nethome/pchoudhary32/CS6210_Project2/openmp/dissemination/results/logd_$n.txt
done
#eof
