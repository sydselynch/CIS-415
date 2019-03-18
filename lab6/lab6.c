#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>


#define THREAD_COUNT 4

char *messages[] = {"Hello","World","Test","String",NULL};
int thread_count = THREAD_COUNT;
pthread_t pthreads[THREAD_COUNT]; 

int memory_size = 0;
volatile char *memory = "StartingMessage";



void routine() {
	sleep(1);
}

void set_message(char *message)
{
	memory = message;
}

char *get_message()
{
	char *returnValue = memory;
	return returnValue;
}

void *pthread_proc(void *ptr)
{	
	char *message = (char*)ptr;
	
	// check message... ?
	// print the message, and thread id.
	printf("Thread Proc - Thread ID: %d, message: %s\n", pthread_self(), message);

	int exit_count = 1000;
	while(exit_count-- > 0)
	{
		// set the message.
		// get the message.
		// print the messages, and thread id. if they are different.
		printf("Thread Proc in loop - ID: %d, message: %s, global message: %s\n", pthread_self(), message, get_message());
		set_message(message);
		message = get_message();
		
	}
	return NULL;
}

void create_threads()
{
	// create a thread passing 1 message each.
	// for each:  print the thread id and the message passed. 
	int i;
	int p;
	for (i = 0; i < THREAD_COUNT; i++) {
		p = pthread_create(&pthreads[i], NULL, pthread_proc, messages[i]);
		if (p) {
			printf("Error\n");
			exit(-1);
		}
		printf("~~~~Thread ID: %d Message: %s\n", pthreads[i], messages[i]);
	}


}

void wait_for_threads_to_exit()
{
	// spin in a loop, wait for each thread to exit.
	int i;
	for (i = 0; i < thread_count; i++) {
		pthread_join(pthreads[i], NULL);
		routine();
		printf("thread %d exited\n", pthreads[i]);
	}
}

int main(int argc, char *argv[])
{
	create_threads();
	wait_for_threads_to_exit();
	return 0;
}
