#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

typedef struct _command
{
	char *cmd;
	char *args[];
}Command;


Command arg0 = {"ls", {"ls","-a",NULL} };
Command arg1 = {"sleep", {"sleep","10",NULL} };
Command argN = {NULL, {NULL} };


Command *Commands[] = {&arg0,&arg1,&argN};


void print_command(Command *command)
{
	int index =0;
	char *ptr = command->args[index];

	printf("Command : ");
	while(ptr != NULL)
	{
		printf("%s ",ptr);
		index = index + 1;
		ptr = command->args[index];
	}
	printf("\n");
}

// use this volatile variable to relay information from signal handler.
volatile int run = 0;

void execute_command(Command *command)
{
	// part 2: implement fork.
	// part 2: exit if the process failed to be created. 
	// part 2: print the parent pid and child pid from parent.
	// part 2: print the parent pid and child pid from child. 
	// part 2: send sigusr1 from parent.
	// part 2: set the run /volatile variable in the sighandler.
	// part 2: wait for run / volatile variable in child.
	// part 2: wait for the child to exit in the parent.
	pid_t pid = fork();
	if (pid == 0) {

	}
	else if (pid > 0) {
	
	}
	else {
		return;
	}


	// part 3: implement execvp on the command in the child process.
	// part 3: cleanup and exit if execvp fails.
	// part 3: if the child exits with a non-0 return code, exit with that code.
	// part 3: if the child exits from a signal, print the reason, exit with -1.
	// part 3: remove the infinite loop in main. 

}

void execute_sequence()
{
	int i =0;
	Command *cmd =  Commands[i];
	
	while( cmd != &argN)
	{
		print_command(cmd);
		execute_command(cmd);
		i = i +1;		
		cmd =  Commands[i];
	}
}

void execute_loop(int loop_count)
{
	int i =0;

	for(i =0; i < loop_count; i++)
	{
		printf("Executing batch loop: %d\n", i);
		execute_sequence();
	}
}

void signal_handler(int signo)
{
	// trap all the signals here. 
	// print a meaningful message for each signal.
	switch (signo) {
		case SIGHUP:
			printf("Recieved signal SIGHUP in process %d\n",(int)getpid());
			break;
		case SIGINT:
			printf("Recieved signal SIGINT in process %d\n",(int)getpid());
			break;
		case SIGQUIT:
			printf("Recieved signal SIGQUIT in process %d\n",(int)getpid());
			break;
		case SIGILL:
			printf("Recieved signal SIGILL in process %d\n",(int)getpid());
			break;
		case SIGABRT:
			printf("Recieved signal SIGABRT in process %d\n",(int)getpid());
			break;
		case SIGFPE:
			printf("Recieved signal SIGFPE in process %d\n",(int)getpid());
			break;
		case SIGSEGV:
			printf("Recieved signal SIGSEGV in process %d\n",(int)getpid());
			break;
		case SIGPIPE:
			printf("Recieved signal SIGPIPE in process %d\n",(int)getpid());
			break;
		case SIGALRM:
			printf("Recieved signal SIGALRM in process %d\n",(int)getpid());
			break;
		case SIGTERM:
			printf("Recieved signal SIGTERM in process %d\n",(int)getpid());
			break;
		case SIGCHLD:
			printf("Recieved signal SIGCHLD in process %d\n",(int)getpid());
			break;
		case SIGUSR1:
			printf("Recieved signal SIGUSR1 in process %d\n",(int)getpid());
			break;
		case SIGUSR2:
			printf("Recieved signal SIGUSR2 in process %d\n",(int)getpid());
			break;
		default:
			break;

	}


	printf("Recieved signal %d in process %d\n",signo,(int)getpid());

}

void subscribe_to_signals()
{
	// call the system to trap the signals in the signal handler here.
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGPIPE, signal_handler);
	signal(SIGALRM, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGCHLD, signal_handler);
	signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);

}

int main(int argc, char *argv[])
{
	subscribe_to_signals();
	execute_loop(2);

	while(1)
	{
		pause();
	}
	return 0;
}
