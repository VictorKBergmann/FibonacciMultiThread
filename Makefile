CC = g++
CFLAGS = -Wall -g
LDFLAGS = -lm

all: library.o fatorial fibonacci quickSort

library.o : library.h
	${CC} ${CFLAGS} -c library.cpp -lpthread

fatorial : library.o fatorial.o
	${CC} ${CFLAGS} library.o fatorial.o ${LDFLAGS} -o fatorial -lpthread
fibonacci : library.o fibonacci.o
	${CC} ${CFLAGS} library.o fibonacci.o ${LDFLAGS} -o fibonacci -lpthread
quickSort : library.o quickSort.o
	${CC} ${CFLAGS} library.o quickSort.o ${LDFLAGS} -o quickSort -lpthread

clean:
	rm -rf *.o
	- rm quickSort
	- rm fibonacci
	- rm fatorial
