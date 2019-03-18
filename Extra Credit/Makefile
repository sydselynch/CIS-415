all: include_crawler.o hashmap.o iterator.o tshashmap.o tsiterator.o tslinkedlist.o tstreeset.o tsuqueue.o linkedlist.o treeset.o uqueue.o arraylist.o tsarraylist.o
	gcc -W -Wall -g -o include_crawler include_crawler.c hashmap.o iterator.o tshashmap.o tsiterator.o tslinkedlist.o tstreeset.o tsuqueue.o linkedlist.o treeset.o uqueue.o arraylist.o tsarraylist.o -lpthread


include_crawler.o:
	gcc -W -Wall -g -c include_crawler.c

hashmap.o:
	gcc -W -Wall -g -c hashmap.c

iterator.o:
	gcc -W -Wall -g -c iterator.c

tshashmap.o:
	gcc -W -Wall -g -c tshashmap.c

tsiterator.o:
	gcc -W -Wall -g -c tsiterator.c

tslinkedlist.o:
	gcc -W -Wall -g -c tslinkedlist.c

tstreeset.o:
	gcc -W -Wall -g -c tstreeset.c

tsuqueue.o:
	gcc -W -Wall -g -c tsuqueue.c

linkedlist.o:
	gcc -W -Wall -g -c linkedlist.c

treeset.o:
	gcc -W -Wall -g -c treeset.c

uqueue.o:
	gcc -W -Wall -g -c uqueue.c

tsarraylist.o:
	gcc -W -Wall -g -c tsarraylist.c

arraylist.o:
	gcc -W -Wall -g -c arraylist.c

clean:
	rm *.o
