#!/bin/sh
#PBS -q cs6210
#PBS -l nodes=1:fourcore
#PBS -l walltime=00:05:00
#PBS -N sense_reversal

for i in {2..8}
do
	./sense_reversal 100 $n
done
#eof
