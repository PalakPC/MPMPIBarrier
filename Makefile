CC = cc
CFLAGS = -g -O0
P = 1
.PHONY: runstream

runstream:
	$(CC) $(CFLAGS) -o openmp_sense_reversal -fopenmp openmp_sense_reversal.c
