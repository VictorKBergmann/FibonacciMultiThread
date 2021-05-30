
#CC = g++
#CCFLAGS = -Wall

#example: fatorial.o
#		$(CC) $(CCFLAGS) -o fatorial fatorial.o -pthread

#example.o: example.cpp
#		$(CC) $(CCFLAGS) -c fatorial.cpp -pthread

#clean:
#		- rm *.o
#		- rm fatorial



#CC = g++
#CCFLAGS = -Wall

#example: quickSort.o
#		$(CC) $(CCFLAGS) -o quickSort quickSort.o -pthread

#example.o: quickSort.cpp
#		$(CC) $(CCFLAGS) -c quickSort.cpp -pthread

#clean:
#		- rm *.o
#		- rm quickSort

compileMerge:
		gcc mergesort.c -o mergesort -lpthread
		g++ fibonnaci.cpp library.cpp -o fibonnaci -lpthread

