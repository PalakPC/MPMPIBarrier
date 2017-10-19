TEST_SRC  = mcs.c mcs-test.c mytime.c
TEST_OBJS = $(TEST_SRC:.cpp=.o)

all: $(TEST_SRC) mcs

%.o: %.c
	/opt/openmpi-1.4.3-gcc44/bin/mpicc -g -Wall -c -fopenmp -o $@ $< -lm 

mcs: $(TEST_OBJS) 
	/opt/openmpi-1.4.3-gcc44/bin/mpicc -g -Wall -fopenmp -o $@ $^ -lm

clean: 
	rm mcs
