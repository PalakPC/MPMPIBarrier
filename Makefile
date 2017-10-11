CC = cc
CFLAGS = -g -O0
P = 1
.PHONY: runstream

runstream:
	$(CC) $(CFLAGS) -DP=$(P) -o brown -fopenmp openmp_centralized3.c -lm
	./brown
