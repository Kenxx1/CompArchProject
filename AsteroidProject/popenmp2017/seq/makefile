# the compiler: gcc for C program, define as g++ for C++
CC = g++
# compiler flags: 
CFLAGS  = -std=c++14 -Wall -Wextra -Wno-deprecated -Werror -pedantic -pedantic-errors

nasteroids-seq: nasteroid-seq.o
	$(CC) $(CFLAGS) -o nasteroids-seq

nasteroids-seq.o: nasteroids-seq.cpp
	$(CC) $(CFLAGS) -c nasteroids-seq.cpp


clean:
	rm -r *.o nasteroids-seq
