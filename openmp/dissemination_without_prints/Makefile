TEST_SRC  = dissemination.c dissemination-test.c
TEST_OBJS = $(TEST_SRC:.cpp=.o)

all: $(TEST_SRC) dissemination

%.o: %.c
	gcc -g -Wall -c -fopenmp -o $@ $< -lm 

dissemination: $(TEST_OBJS) 
	gcc -g -Wall -fopenmp -o $@ $^ -lm

clean: 
	rm dissemination
