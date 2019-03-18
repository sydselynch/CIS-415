#include <signal.h>
#include <stdio.h>
#include <time.h>

#define UNUSED __attribute__((unused))

volatile int USR1_received = 0;		/* 0 means it is NOT true */

void onusr1(UNUSED int sig) {
    USR1_received++;			/* anything non-zero is true */
}

int main(UNUSED int argc, UNUSED char *argv[]) {
    struct timespec tm = {0, 20000000};	/* 20,000,000 ns == 20 ms */

    if (signal(SIGUSR1, onusr1) == SIG_ERR) {
        fprintf(stderr, "Can't establish SIGUSR1 handler\n");
        return 1;
    }
/*
 * now wait for the signal
 */
    while (! USR1_received)
        (void)nanosleep(&tm, NULL);
    return 0;
}