TEST_SRC  = mcs.c mcs-test.c mytime.c
TEST_OBJS = $(TEST_SRC:.cpp=.o)

all: $(TEST_SRC) mcs

%.o: %.c
	mpicc -g -Wall -c -o $@ $< -lm 

mcs: $(TEST_OBJS) 
	mpicc -g -Wall -o $@ $^ -lm

clean: 
	rm mcs
