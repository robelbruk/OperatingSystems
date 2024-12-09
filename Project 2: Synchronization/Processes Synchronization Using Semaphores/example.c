#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

#define NLOOP 10
#define INITIAL_BALANCE 0

int main(int argc, char **argv) {
    int fd, *bank_account;
    sem_t *mutex;

    // Open a file and map it into memory to hold the shared BankAccount
    fd = open("bank_account.txt", O_RDWR | O_CREAT, S_IRWXU);
    if (fd == -1) {
        perror("Failed to open file");
        exit(1);
    }

    write(fd, INITIAL_BALANCE, sizeof(int));
    bank_account = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (bank_account == MAP_FAILED) {
        perror("Failed to map memory");
        close(fd);
        exit(1);
    }
    close(fd);

    // Create and initialize the semaphore
    if ((mutex = sem_open("bank_semaphore", O_CREAT, 0644, 1)) == SEM_FAILED) {
        perror("Semaphore initialization");
        exit(1);
    }

    srand(time(NULL));

    if (fork() == 0) { /* Child Process: Poor Student */
        for (int i = 0; i < NLOOP; i++) {
            printf("Poor Student: Attempting to Check Balance\n");

            sem_wait(mutex); // Enter critical section
            int local_balance = *bank_account;

            int random_num = rand();
            if (random_num % 2 == 0) {
                int need = rand() % 51; // Random amount needed between 0-50
                printf("Poor Student needs $%d\n", need);
                if (need <= local_balance) {
                    local_balance -= need;
                    *bank_account = local_balance;
                    printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, local_balance);
                } else {
                    printf("Poor Student: Not Enough Cash ($%d)\n", local_balance);
                }
            } else {
                printf("Poor Student: Last Checking Balance = $%d\n", local_balance);
            }
            sem_post(mutex); // Exit critical section
            sleep(rand() % 6); // Sleep 0-5 seconds
        }
        exit(0);
    }

    /* Parent Process: Dear Old Dad */
    for (int i = 0; i < NLOOP; i++) {
        printf("Dear Old Dad: Attempting to Check Balance\n");

        sem_wait(mutex); // Enter critical section
        int local_balance = *bank_account;

        int random_num = rand();
        if (random_num % 2 == 0) {
            if (local_balance < 100) {
                int amount = rand() % 101; // Random deposit between 0-100
                if (amount % 2 == 0) {
                    local_balance += amount;
                    *bank_account = local_balance;
                    printf("Dear Old Dad: Deposits $%d / Balance = $%d\n", amount, local_balance);
                } else {
                    printf("Dear Old Dad: Doesn't have any money to give\n");
                }
            } else {
                printf("Dear Old Dad: Thinks Student has enough Cash ($%d)\n", local_balance);
            }
        } else {
            printf("Dear Old Dad: Last Checking Balance = $%d\n", local_balance);
        }
        sem_post(mutex); // Exit critical section
        sleep(rand() % 6); // Sleep 0-5 seconds
    }

    // Clean up
    sem_close(mutex);
    sem_unlink("bank_semaphore");
    munmap(bank_account, sizeof(int));
    unlink("bank_account.txt");
    return 0;
}
