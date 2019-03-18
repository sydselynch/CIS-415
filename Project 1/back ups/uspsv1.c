/*int read = 1;
					while (read != 0) {
						read = p1getline(fd, buf, size);
						offset += read;
						printf("line is %s\n", buf);
						lseek(fd, offset, SEEK_SET);
						int wordread = 0;
						int i = 0;
						while (wordread != -1) {
							wordread = p1getword(buf, wordread, word);
							printf("%d word is: %s\n", i, word);
							//printf("length is: %d\n", p1strlen(word));
							//i++;
						}
					}*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "uqueue.h"
#include "p1fxns.h"
#include <string.h>
#include <sys/wait.h>


// usage: ./uspsv? [–-quantum=<msec>] [workload_file] 

int QUANTUM_VALUE;
UQueue *queue;

typedef struct Argument {
	char *args;
	int size;
	int pid;
} argument;

struct Argument *argument_create(char *args, int num_args) {
	struct Argument *argument = (struct Argument *)malloc(sizeof(struct Argument) + 1);
	if (argument != NULL) {
		argument->args = malloc(sizeof(char) * num_args);
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

void parse_args(int argc, char *argv[]) {
	int index;
	int fd;
	char buf[1024];
	char word[1024];
	
	// make this dyanmic
	char *args[1024];
	pid_t pid[1024];
	int size = 1024;
	int pid_index = 0;


	// if FD = 0, read from stdin

	if (argc > 3) {
		printf("too many args\n");
	}
	else {
		for (index = 1; index < argc; index++) {
			char *arg = argv[index];
			if (p1strneq(argv[index], "--quantum=", 10)) {
				arg = arg + 10;
				if (p1atoi(arg) < 1) {
					if (getenv("USPS_QUANTUM_MSEC") == NULL) {
					QUANTUM_VALUE = p1atoi(arg);
					}
					else {
						QUANTUM_VALUE = p1atoi(getenv("USPS_QUANTUM_MSEC"));
					}
				}
				else {
					QUANTUM_VALUE = p1atoi(arg);
				}
				
				printf("Quantum value is: %d\n", QUANTUM_VALUE);
			}
			else {
				//printf(arg);
				fd = open(arg, O_RDONLY);
				printf("%d", fd);
				if (fd == -1) {
					printf("file failed to open");
				}
				else if (fd == 1) {
					printf("reading from stdin\n");
				}
				else {
					int i, j;
					//char **args;
					while (p1getline(fd, buf, size) != 0) {
						//printf("%s\n", buf);
						i = p1getword(buf, i, word);
						//args = NULL;
						j = 0;
						//args = malloc(sizeof(buf));
						while (i != -1) {
							//printf("%s\n", word);
							args[j] = p1strdup(word);
							printf("word: %s\n", args[j]);
							j++;
							//uq_add(queue, args);
							i = p1getword(buf, i, word);
						}
						strip_newline(args[j-1]);
						i = 0;
						printf("%s\n", args[0]);
						// Fork proceses
						pid[pid_index] = fork();
						if (pid[pid_index] < 0) {
							printf("fork failed");
						}
						else if (pid[pid_index] == 0) {
							printf("child success\n");
							if (execvp(args[0], args) == -1) {
								printf("Execvp failure\n");
							}
						}
						else {
							printf("parent process\n");
						}
						pid_index++;
					}
					
					for (int k = 0; k < pid_index; k++) {

						int status;
						printf("waiting...\n");
						waitpid(pid[k], &status, 0);
						printf("done waiting\n");
					}

					printf("end of file\n");
				}
			}
		}
	}
}


int main(int argc, char *argv[]) {
	//UQueue *uq = uq_create();
	queue = uq_create();
	parse_args(argc, argv);
	return 0;
}