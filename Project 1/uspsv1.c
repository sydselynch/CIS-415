#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include "p1fxns.h"
#include "arraylist.h"

/*
Authorship Statement:
Syd Lynch
slynch2
CIS 415
Project 1

This is my own work except for the ADTs uqueue, iterator, arraylist, and linkedlist,
which were created by Joe Sventek.

I discussed the project with other CIS 415 students Danny Lu and Jeremy Unck, but all
code was written by me.

*/

int QUANTUM_VALUE;

typedef struct argument {
	char **args;
	int size;
} Argument;

Argument *argument_create(char **args, int num_args) {
	Argument *argument = (Argument *)malloc(sizeof(Argument) + 1);
	if (argument != NULL) {
		argument->args = args;
		argument->size = num_args;
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
