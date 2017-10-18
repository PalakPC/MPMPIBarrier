TEST_SRC  = combined.c combined-test.c dissemination.c mcs.c
TEST_OBJS = $(TEST_SRC:.cpp=.o)

all: $(TEST_SRC) combined

%.o: %.c
	/opt/openmpi-1.4.3-gcc44/bin/mpicc -g -Wall -c -fopenmp -o $@ $< -lm 

combined: $(TEST_OBJS) 
	/opt/openmpi-1.4.3-gcc44/bin/mpicc -g -Wall -fopenmp -o $@ $^ -lm

clean: 
	rm combined
