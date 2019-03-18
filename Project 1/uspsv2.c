#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include "p1fxns.h"
#include "arraylist.h"



int QUANTUM_VALUE;
struct timespec tm = {0, 20000000};
volatile int SIGUSR1_received = 0;

typedef struct argument {
	char **args;
	int size;
	pid_t pid;
	int SIGUSR1_received;
} Argument;

void sigonusr1() {
	SIGUSR1_received++;
}

Argument *argument_create(char **args, int num_args) {
	Argument *argument = (Argument *)malloc(sizeof(Argument) + 1);
	if (argument != NULL) {
		argument->args = args;
		argument->size = num_args;
		argument->SIGUSR1_received = 0;
	}

	return argument;
}

void destroy_argument(Argument *argument) {
	int i;
	for (i = 0; i < argument->size; i++) {
		free(argument->args[i]);
	}
	free(argument->args);
	free(argument);
}

void set_pid(Argument *arg, int pid) {
	arg->pid = pid;
}

void set_signal(Argument *arg) {
	arg->SIGUSR1_received++;
}

void strip_newline(char *word) {
	int i;
	int length = p1strlen(word);
	for (i = 0; i < length; i++) {
		if (word[i] == '\n' || word[i] == '\r') {
			word[i] = '\0';
		}
	}
}

ArrayList *parse_args(int argc, char *argv[]) {
	int index;
	int fd = 0;
	int i = 0;
	int j = 0;
	char line_buffer[1024];
	char word_buffer[100];
	int size = 1024;
	char **args;
	ArrayList *arr = al_create(0);

	if (getenv("USPS_QUANTUM_MSEC") != NULL) {
		QUANTUM_VALUE = p1atoi(getenv("USPS_QUANTUM_MSEC"));
	}

	if (argc > 3) {
		p1perror(2, "Invalid usage, too many arguments\n");
		return NULL;
	}
	else {
		for (index = 1; index < argc; index++) {
			char *arg = argv[index];

			// Process Quantum value
			if (p1strneq(arg, "--quantum=", 10)) {
				arg = arg + 10;
				if (p1atoi(arg) < 1) {
					p1perror(2, "Invalid quantum entered\n");
					return NULL;
				}
				else {
					QUANTUM_VALUE = p1atoi(arg);
				}
			}
			else {
				fd = open(arg, O_RDONLY);
			}
		}
		if (fd == -1) {
				p1perror(2, "File failed to open.\n");
		}
		else {
			while (p1getline(fd, line_buffer, size) != 0) {
				i = p1getword(line_buffer, i, word_buffer);
				j = 0;
				args = malloc(sizeof(char*)*p1strlen(line_buffer));
				while (i != -1) {
					strip_newline(word_buffer);
					args[j] = p1strdup(word_buffer);
					j++;
					i = p1getword(line_buffer, i, word_buffer);
				}

				args[j] = NULL;
				// Create struct that holds line of args
				Argument *current_arg;
				current_arg = argument_create(args, j);
				al_add(arr, current_arg);
				i = 0;
			}
		}
	}

	return arr;
}

void execute_processes(ArrayList *arr) {
	if (arr == NULL) {
		return;
	}
	int num_args = al_size(arr);
	int num_processes = 0;
	pid_t pid[num_args];
	int i;

	if (signal(SIGUSR1, sigonusr1) == SIG_ERR) {
			p1perror(2, "SIGUSR error\n");
			return;
		}
	for (i = 0; i < num_args; i++) {
		void *current_arg;
		al_get(arr, i, &current_arg);
		current_arg = (Argument*)current_arg;
		pid[i] = fork();

		if (pid[i] < 0) {
			p1perror(2, "Error forking\n");
		}
		else if (pid[i] == 0) {
			num_processes++;
			while (!SIGUSR1_received) {
				(void)nanosleep(&tm, NULL);
			}
			if (execvp(((Argument*)current_arg)->args[0], ((Argument*)current_arg)->args) == -1) {
				al_destroy(arr, (void *)destroy_argument);
				return;
			}
		}
		else {
			// Child success
		}
	}

	for (i = 0; i < num_args; i++) {
		kill(pid[i], SIGUSR1);
	}

	for (i = 0; i < num_args; i++) {
		if (pid[i] != 0) {
			kill(pid[i], SIGSTOP);
		}
	}

	for (i = 0; i < num_args; i++) {
		if (pid[i] != 0) {
			kill(pid[i], SIGCONT);
		}
	}

	for (i = 0; i < num_args; i++) {
		int status;
		waitpid(pid[i], &status, 0);
	}
	al_destroy(arr, (void *)destroy_argument);
}


int main(int argc, char *argv[]) {
	ArrayList *arr = parse_args(argc, argv);
	execute_processes(arr);
	return 0;
}
