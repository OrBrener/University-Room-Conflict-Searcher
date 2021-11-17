CC=clang
CFLAGS=-std=c99 -Wall -pedantic

a6: a6.o hash.o hash.h
	$(CC) $(CFLAGS) a6.o hash.o -o a6
a6.o: a6.c
	$(CC) $(CFLAGS) -c a6.c
clean:
	rm *.o a6

