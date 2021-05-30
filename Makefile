CC = g++
CCFLAGS = -Wall


exemple1: fatorial.o
		$(CC) fatorial.cpp library.cpp -o fatorial fatorial.o -pthread

example1.o: fatorial.cpp
		$(CC) fatorial.cpp library.cpp -c fatorial.cpp -pthread

example2: fibonacci.o
		$(CC) fibonacci.o library.cpp -o fibonacci fibonacci.o -pthread

example2.o: fibonacci.cpp
		$(CC) fibonacci.cpp library.cpp -c fibonacci.cpp -pthread

example3: quickSort.o
		$(CC) quickSort.o library.cpp -o quickSort quickSort.o -pthread

example3.o: quickSort.cpp
		$(CC) $(CCFLAGS) library.cpp -c quickSort.cpp -pthread

clean:
		- rm *.o
		- rm quickSort
		- rm fibonacci
		- rm fatorial