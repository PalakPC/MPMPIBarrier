#!/bin/sh
#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N disssemination

for n in {2..8}
do
	./dissemination 1000 $n > /nethome/pchoudhary32/CS6210_Project2/openmp/dissemination_without_printfs/results/logd.txt
done
#eof
