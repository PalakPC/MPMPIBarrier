#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:05:00
#PBS -N disssemination

for n in {2..12}
do
	./dissemination 4 $n > /nethome/pchoudhary32/CS6210_Project2/openmp/dissemination/results/logd$n.txt
done
#eof
