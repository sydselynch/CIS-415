CC = gcc
CFLAGS = -W -Wall -g -c

all: uspsv1.o uspsv2.o uspsv3.o uspsv4.o p1fxns.o arraylist.o uqueue.o iterator.o linkedlist.o
	gcc -W -Wall -g -o uspsv1 uspsv1.c p1fxns.o arraylist.o iterator.o
	gcc -W -Wall -g -o uspsv2 uspsv2.c p1fxns.o arraylist.o iterator.o
	gcc -W -Wall -g -o uspsv3 uspsv3.c p1fxns.o arraylist.o uqueue.o iterator.o linkedlist.o
	gcc -W -Wall -g -o uspsv4 uspsv4.c p1fxns.o arraylist.o uqueue.o iterator.o linkedlist.o

uspsv1.o: uspsv1.c
	gcc -W -Wall -g -c uspsv1.c

uspsv2.o: uspsv2.c
	gcc -W -Wall -g -c uspsv2.c

uspsv3.o: uspsv3.c
	gcc -W -Wall -g -c uspsv3.c

uspsv4.o: uspsv4.c
	gcc -W -Wall -g -c uspsv4.c

p1fxns.o: p1fxns.c p1fxns.h
	gcc -W -Wall -g -c p1fxns.c

uqueue.o: uqueue.c uqueue.h
	gcc -W -Wall -g -c uqueue.c

iterator.o: iterator.c iterator.h
	gcc -W -Wall -g -c iterator.c

linkedlist.o: linkedlist.c linkedlist.h
	gcc -W -Wall -g -c linkedlist.c

arraylist.o: arraylist.c arraylist.h
	gcc -W -Wall -g -c arraylist.c

clean:
	rm *.o

