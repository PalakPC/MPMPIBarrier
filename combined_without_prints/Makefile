TEST_SRC  = combined.c combined-test.c dissemination.c mcs.c mytime.c
TEST_OBJS = $(TEST_SRC:.cpp=.o)

all: $(TEST_SRC) combined

%.o: %.c
	mpicc -g -Wall -c -fopenmp -o $@ $< -lm 

combined: $(TEST_OBJS) 
	mpicc -g -Wall -fopenmp -o $@ $^ -lm

clean: 
	rm combined
