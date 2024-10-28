/* timer.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int alarms_count = 0;  // Counts the number of SIGALRM signals received
time_t start_time;     // Tracks when the program started

void alarm_handler(int signum) { // SIGALRM handler
    alarms_count++;
    alarm(1); // Schedule the next alarm in 1 second
}

void sigint_handler(int signum) { // SIGINT handler for CTRL-C
    time_t end_time = time(NULL); // Current time
    printf("\nTotal execution time: %ld seconds\n", end_time - start_time);
    printf("Total alarms received: %d\n", alarms_count);
    exit(0); // Exit the program
}

int main(int argc, char * argv[]) {
    start_time = time(NULL); // Record start time

    // Register signal handlers
    signal(SIGALRM, alarm_handler); // Handle SIGALRM
    signal(SIGINT, sigint_handler); // Handle SIGINT (CTRL-C)

    alarm(1); // Schedule the first SIGALRM in 1 second

    while (1) {
        pause(); // Wait for signal
    }

    return 0;
}
