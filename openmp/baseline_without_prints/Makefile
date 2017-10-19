TEST_SRC  = baseline.c mytime.c
TEST_OBJS = $(TEST_SRC:.cpp=.o)

all: $(TEST_SRC) baseline

%.o: %.c
	gcc -g -Wall -c -fopenmp -o $@ $< -lm 

baseline: $(TEST_OBJS) 
	gcc -g -Wall -fopenmp -o $@ $^ -lm

clean: 
	rm baseline
