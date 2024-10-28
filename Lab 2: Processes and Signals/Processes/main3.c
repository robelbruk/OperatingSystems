#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

void child_process() {
    int pid = getpid();
    int ppid = getppid();
    int iterations = rand() % 30 + 1;  // random number of iterations, max 30

    for (int i = 0; i < iterations; i++) {
        printf("Child Pid: %d is going to sleep!\n", pid);
        
        int sleep_time = rand() % 10 + 1;  //  sleep time, max 10 seconds
        sleep(sleep_time);
        
        printf("Child Pid: %d is awake!\nWhere is my Parent: %d?\n", pid, ppid);
    }
    exit(0);
}

int main() {
    pid_t pid1, pid2;

    // Seed the rand number generator with the current time
    srand(time(NULL));

    // Create the first child process
    pid1 = fork();
    if (pid1 == 0) {
        // Inside the first child process
        child_process();
    }

    // Create the second child process
    pid2 = fork();
    if (pid2 == 0) {
        // Inside the second child process
        child_process();
    }

    // In the parent process
    int status;
    pid_t finished_pid;

    while ((finished_pid = wait(&status)) > 0) {
        printf("Child Pid: %d has completed\n", finished_pid);
    }

    return 0;
}
