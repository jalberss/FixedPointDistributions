CC = gcc-8

.PHONY: clean all

all:
	$(CC) fixed_point.c -o fixed_point
test:
	$(CC) -DTEST fixed_point.c -o fixed_point
clean:
	rm fixed_point *.o
