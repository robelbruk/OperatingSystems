#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

// Function prototypes
void ParentProcess(int sharedMem[]);
void ChildProcess(int sharedMem[]);

int main() {
    int ShmID;
    int *ShmPTR;
    pid_t pid;
    int status;

    // Create shared memory segment for 2 integers
    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }
    printf("Server has received a shared memory of two integers...\n");

    // Attach shared memory segment
    ShmPTR = (int *)shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    printf("Server has attached the shared memory...\n");

    // Initialize shared memory variables
    ShmPTR[0] = 0; // BankAccount
    ShmPTR[1] = 0; // Turn
    printf("Server has initialized BankAccount = %d, Turn = %d\n", ShmPTR[0], ShmPTR[1]);

    // Fork a child process
    printf("Server is about to fork a child process...\n");
    pid = fork();
    if (pid < 0) {
        printf("*** fork error (server) ***\n");
        exit(1);
    } else if (pid == 0) {
        ChildProcess(ShmPTR);
        exit(0);
    } else {
        ParentProcess(ShmPTR);
    }

    // Wait for the child process to complete
    wait(&status);
    printf("Server has detected the completion of its child...\n");

    // Detach and remove shared memory
    shmdt((void *)ShmPTR);
    printf("Server has detached its shared memory...\n");
    shmctl(ShmID, IPC_RMID, NULL);
    printf("Server has removed its shared memory...\n");
    printf("Server exits...\n");
    return 0;
}

void ParentProcess(int sharedMem[]) {
    for (int i = 0; i < 25; i++) {
        sleep(rand() % 6); // Sleep for 0-5 seconds
        int account = sharedMem[0]; // Copy BankAccount to local variable

        // Wait for Turn == 0
        while (sharedMem[1] != 0) {
            // Busy wait (no-op)
        }

        // Deposit logic
        if (account <= 100) {
            int deposit = rand() % 101; // Random deposit amount (0-100)
            if (deposit % 2 == 0) {
                account += deposit;
                printf("Dear old Dad: Deposits $%d / Balance = $%d\n", deposit, account);
            } else {
                printf("Dear old Dad: Doesn't have any money to give\n");
            }
        } else {
            printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
        }

        sharedMem[0] = account; // Update BankAccount
        sharedMem[1] = 1;       // Set Turn to 1
    }
}

void ChildProcess(int sharedMem[]) {
    for (int i = 0; i < 25; i++) {
        sleep(rand() % 6); // Sleep for 0-5 seconds
        int account = sharedMem[0]; // Copy BankAccount to local variable

        // Wait for Turn == 1
        while (sharedMem[1] != 1) {
            // Busy wait (no-op)
        }

        // Withdraw logic
        int withdraw = rand() % 51; // Random withdraw amount (0-50)
        printf("Poor Student needs $%d\n", withdraw);
        if (withdraw <= account) {
            account -= withdraw;
            printf("Poor Student: Withdraws $%d / Balance = $%d\n", withdraw, account);
        } else {
            printf("Poor Student: Not Enough Cash ($%d)\n", account);
        }

        sharedMem[0] = account; // Update BankAccount
        sharedMem[1] = 0;       // Set Turn to 0
    }
}
