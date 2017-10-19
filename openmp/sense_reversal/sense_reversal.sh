#!bin/sh
#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N sense_reversal

for n in {2..8}
do
	/nethome/pchoudhary32/CS6210_Project2/openmp/sens_reversal/sense_reversal 1000 $n > /nethome/pchoudhary32/CS6210_Project2/openmp/sense_reversal/results/logd_$n.txt
done
#eof
