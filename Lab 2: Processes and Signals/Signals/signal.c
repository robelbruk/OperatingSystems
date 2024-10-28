/* signal.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// #1
int alarm_triggered = 0; // Global variable to track if alarm was triggered

void handler(int signum) { // signal handler
    printf("Hello World!\n");
    alarm_triggered = 1; // Set flag to indicate handler has been called
}

int main(int argc, char * argv[]) {
    signal(SIGALRM, handler); // Register handler to handle SIGALRM
    alarm(5); // Schedule a SIGALRM for 5 seconds

    while (!alarm_triggered); // Busy wait until alarm is triggered

    // After the signal handler is done
    printf("Turing was right!\n");
    return 0;
}

// #2
void handler(int signum) { // signal handler
    printf("Hello World!\n");
    alarm(5); // Schedule the next alarm
}

int main(int argc, char * argv[]) {
    signal(SIGALRM, handler); // Register handler to handle SIGALRM
    alarm(5); // Schedule a SIGALRM for 5 seconds

    while (1) { // Infinite loop
        pause(); // Wait for signal
        printf("Turing was right!\n");
    }

    return 0;
}