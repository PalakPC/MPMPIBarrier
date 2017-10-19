TEST_SRC  = baseline.c mytime.c
TEST_OBJS = $(TEST_SRC:.cpp=.o)

all: $(TEST_SRC) baseline

%.o: %.c
	mpicc -g -Wall -c -o $@ $< -lm 

baseline: $(TEST_OBJS) 
	mpicc -g -Wall -o $@ $^ -lm

clean: 
	rm baseline
