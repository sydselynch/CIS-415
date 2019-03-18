#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "p1fxns.h"
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

// usage: ./uspsv? [–-quantum=<msec>] [workload_file] 

int QUANTUM_VALUE;
struct timespec tm = {0, 20000000};
volatile int SIGUSR1_received = 0;
volatile int SIGSTOP_received = 0;
volatile int SIGCONT_received = 0;


void sigonusr1() {
	printf("USR1 received\n");
	SIGUSR1_received++;
}

void sigstop() {
	printf("SIGSTOP received\n");
	SIGSTOP_received++;
}

void sigcont() {
	printf("SIGCONT received\n");
	SIGCONT_received++;
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
	int i = 0;
	int j = 0;

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
				
				printf("%d\n", QUANTUM_VALUE);
			}
			else {
				fd = open(arg, O_RDONLY);
				if (fd == -1) {
					printf("file failed to open");
				}
				else {
					while (p1getline(fd, buf, size) != 0) {
						i = p1getword(buf, i, word);
						j = 0;
						while (i != -1) {
							args[j] = p1strdup(word);
							j++;
							i = p1getword(buf, i, word);
						}
						strip_newline(args[j-1]);
						i = 0;
						pid[pid_index] = fork();
						if (signal(SIGUSR1, sigonusr1) == SIG_ERR) {
							printf("SIGUSR error\n");
							return;
						}
						/*if (signal(SIGSTOP, sigstop) == SIG_ERR) {
							printf("sigstop error\n");
							return;
						}*/
						if (signal(SIGCONT, sigcont) == SIG_ERR) {
							printf("sigcont error\n");
							return;
						}
						//printf("%d signal received\n", getpid());
						if (pid[pid_index] == -1) {
							printf("fork failed");
						}
						else if (pid[pid_index] == 0) {
							printf("%d waiting on signal\n", getpid());
							printf("child success\n");
							while (!SIGUSR1_received) {
								//printf("sig not received");
								(void)nanosleep(&tm, NULL);
							}
							execvp(args[0], args);
						}
						else {
							printf("success\n");
						}
						pid_index++;
					}
					int k = 0;
					printf("parent sending sigs\n");
					for (k = 0; k < pid_index; k++) {
						kill(pid[k], SIGUSR1);
					}
/*					printf("parent sending stop sigs\n");
					for (k = 0; k < pid_index; k++) {
						kill(pid[k], SIGSTOP);
					}
					printf("parent sending cont sigs\n");
					for (k = 0; k < pid_index; k++) {
						kill(pid[k], SIGCONT);
					}*/
					
					for (k = 0; k < pid_index; k++) {
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
	//queue = uq_create();
	parse_args(argc, argv);
	return 0;
}