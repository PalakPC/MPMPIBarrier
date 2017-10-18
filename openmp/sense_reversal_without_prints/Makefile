TEST_SRC  = sense_reversal-test.c sense_reversal.c
TEST_OBJS = $(TEST_SRC:.cpp=.o)

all: $(TEST_SRC) sense_reversal

%.o: %.c
	gcc -g -Wall -c -fopenmp -o $@ $< 

sense_reversal: $(TEST_OBJS) 
	gcc -g -Wall -fopenmp -o $@ $^

clean: 
	rm sense_reversal
