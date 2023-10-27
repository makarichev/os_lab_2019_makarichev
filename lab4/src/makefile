CC=gcc
CFLAGS=-I.

all: process_memory parallel_sum

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max.o utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

process_memory:
	$(CC) -o process_memory.o process_memory.c

parallel_sum: utils.o
	$(CC) -pthread -o parallel_sum.o utils.o parallel_sum.c $(CFLAGS)

clean :
	rm parallel_min_max.o process_memory.o find_min_max.o utils.o parallel_sum.o