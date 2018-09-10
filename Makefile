CC = gcc-8
FLAGS = -Wall

.PHONY: clean all

all:
	$(CC) $(FLAGS) fixed_point.c -o fixed_point
test:
	$(CC) $(FLAGS) -DTEST -DDEBUG_OUTPUT fixed_point.c -o fixed_point
clean:
	rm fixed_point *.o
