TEST_SRC  = baseline.c mytime.c
TEST_OBJS = $(TEST_SRC:.cpp=.o)

all: $(TEST_SRC) baseline

%.o: %.c
	/opt/openmpi-1.4.3-gcc44/bin/mpicc -g -Wall -c -fopenmp -o $@ $< -lm 

baseline: $(TEST_OBJS) 
	/opt/openmpi-1.4.3-gcc44/bin/mpicc -g -Wall -fopenmp -o $@ $^ -lm

clean: 
	rm baseline
