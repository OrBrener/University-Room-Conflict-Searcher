CC=clang
CFLAGS=-std=c99 -Wall -pedantic
EXECS=a6 index get_idx get_string query longdump and set2idx

all: $(EXECS)

clean:
	rm -i $($EXECS) *.o

# override the implicit compilation rules for executables so that we can
# override them with new ones
%: %.o
%: %.c



# compile a5test1.c using the ARRAY flag to use array operations
# a6: hash.o set2idx.o and.o longdump.o query.o get_string.o get_idx.o index.o a6.o
# 	$(CC) $(CFLAGS) hash.o set2idx.o and.o longdump.o query.o get_string.o get_idx.o index.o a6.o -o a6

a6: a6.o hash.o hash.h
	$(CC) $(CFLAGS) a6.o hash.o -o a6

index:  index.o hash.h hash.o
	$(CC) $(CFLAGS) index.o hash.o -o index

get_idx:  get_idx.o hash.h hash.o
	$(CC) $(CFLAGS) get_idx.o hash.o -o get_idx

get_string:  get_string.o hash.h hash.o
	$(CC) $(CFLAGS) get_string.o hash.o -o get_string

query:  query.o hash.h hash.o
	$(CC) $(CFLAGS) query.o hash.o -o query

longdump: longdump.c
	$(CC) $(CFLAGS) longdump.c -o longdump

and: and.c
	$(CC) $(CFLAGS) and.c -o and

set2idx: set2idx.c
	$(CC) $(CFLAGS) set2idx.c -o set2idx

# create .o files by compiling the .c files
# create .o files by compiling the .c files
%.o : %.c hash.h
	$(CC) -c $(CFLAGS) $< -o $@

a5test1: a5test1.o hash.o memsys.o memsys.h hash.h
	$(CC) $(CFLAGS) memsys.o hash.o a5test1.o -o a5test1




