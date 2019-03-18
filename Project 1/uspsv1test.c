#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#include "p1fxns.h"
#include "arraylist.h"

int QUANTUM_VALUE;

typedef struct Argument {
	char **args;
	int size;
} argument;

struct Argument *argument_create(char **args, int num_args) {
	struct Argument *argument = (struct Argument *)malloc(sizeof(struct Argument) + 1);
	if (argument != NULL) {
		argument->args = args;
		argument->size = num_args;
	}

	return argument;
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
	int fd;
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
		printf("Too many arguments\n");
		return NULL;
	}
	else {
		for (index = 1; index < argc; index++) {
			char *arg = argv[index];
			
			// Process Quantum value
			if (p1strneq(arg, "--quantum=", 10)) {
				arg = arg + 10;
				if (p1atoi(arg) < 1) {
					printf("Invalid quantum entered\n");
				}
				else {
					QUANTUM_VALUE = p1atoi(arg);
				}
			}
			else {
				fd = open(arg, O_RDONLY);
				printf("%d\n", fd);
				if (fd == -1) {
					printf("File failed to open\n");
				}
				else if (fd == 0) {
					printf("Reading from stdin\n");
				}
				else {
					while (p1getline(fd, line_buffer, size) != 0) {
						i = p1getword(line_buffer, i, word_buffer);
						j = 0;
						args = malloc(sizeof(char*)*p1strlen(line_buffer));
						while (i != -1) {
							strip_newline(word_buffer);
							args[j] = p1strdup(word_buffer);
							//printf("%s\n", args[j]);
							printf("Word is: %s\n", word_buffer);
							j++;
							i = p1getword(line_buffer, i, word_buffer);
						}
						// Create struct that holds line of args
						struct Argument *current_arg;
						current_arg = argument_create(args, j);
						al_add(arr, current_arg);
						void *arg_test;
						al_get(arr, 0, &arg_test);
						//(struct Argument*)arg_test;
						//printf("%s\n", ((struct Argument*)arg_test)->args[0]);
						//printf("%ld\n", al_size(arr));
						i = 0;
					}
				}
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
	//printf("Size: %d", num_args);
	for (i = 0; i < num_args; i++) {
		void *current_arg;
		al_get(arr, i, &current_arg);
		current_arg = (struct Argument*)current_arg;
		pid[i] = fork();
		if (pid[i] < 0) {
			printf("Error forking\n");
		}
		else if (pid[i] == 0) {
			printf("Child success\n");
			num_processes++;
			printf("Trying to execute: %s\n", ((struct Argument*)current_arg)->args[0]);
			if (execvp(((struct Argument*)current_arg)->args[0], ((struct Argument*)current_arg)->args) == -1) {
				printf("execvp failure\n");
			}
		}
		else {
			printf("parent process\n");
		}
	}

	while (num_processes > 0) {
		int status;
		printf("Waiting...\n");
		waitpid(pid[i], &status, 0);
		num_processes--;
	}

}


int main(int argc, char *argv[]) {
	ArrayList *arr = parse_args(argc, argv);
	execute_processes(arr);
	return 0;
}