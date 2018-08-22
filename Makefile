
.PHONY: clean all

all:
	cc fixed_point.c -o fixed_point
test:
	cc -DTEST fixed_point.c -o fixed_point
clean:
	rm fixed_point *.o
