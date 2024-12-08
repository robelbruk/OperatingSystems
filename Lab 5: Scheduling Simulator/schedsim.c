#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

// Process structure definition
typedef struct {
    int pid;    // Process ID
    int bt;     // Burst Time
    int pri;    // Priority
    int art;    // Arrival Time
    int wt;     // Waiting Time
    int tat;    // Turnaround Time
} ProcessType;

// Function prototypes
void findWaitingTimeFCFS(ProcessType plist[], int n);
void findWaitingTimeSJF(ProcessType plist[], int n);
void findWaitingTimeRR(ProcessType plist[], int n, int quantum);
void findWaitingTimePriority(ProcessType plist[], int n);
void findTurnAroundTime(ProcessType plist[], int n);
void findavgTimeFCFS(ProcessType plist[], int n);
void findavgTimeSJF(ProcessType plist[], int n);
void findavgTimeRR(ProcessType plist[], int n, int quantum);
void findavgTimePriority(ProcessType plist[], int n);
void printMetrics(ProcessType plist[], int n);
ProcessType* parse_file(FILE *input_file, int *n);

// FCFS
void findWaitingTimeFCFS(ProcessType plist[], int n) {
    plist[0].wt = 0; // First process has no waiting time
    for (int i = 1; i < n; i++) {
        plist[i].wt = plist[i - 1].bt + plist[i - 1].wt;
    }
}

// SJF
void findWaitingTimeSJF(ProcessType plist[], int n) {
    int completed = 0, t = 0, min_bt = INT_MAX, shortest = 0, finish_time;
    int *remaining_bt = malloc(n * sizeof(int));
    int check = 0;

    for (int i = 0; i < n; i++) {
        remaining_bt[i] = plist[i].bt;
    }

    while (completed != n) {
        for (int i = 0; i < n; i++) {
            if ((plist[i].art <= t) && (remaining_bt[i] < min_bt) && remaining_bt[i] > 0) {
                min_bt = remaining_bt[i];
                shortest = i;
                check = 1;
            }
        }

        if (check == 0) {
            t++;
            continue;
        }

        remaining_bt[shortest]--;
        min_bt = remaining_bt[shortest] > 0 ? remaining_bt[shortest] : INT_MAX;

        if (remaining_bt[shortest] == 0) {
            completed++;
            finish_time = t + 1;
            plist[shortest].wt = finish_time - plist[shortest].bt - plist[shortest].art;
            if (plist[shortest].wt < 0) plist[shortest].wt = 0;
        }
        t++;
    }
    free(remaining_bt);
}

// RR
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) {
    int *remaining_bt = malloc(n * sizeof(int));
    int t = 0;

    for (int i = 0; i < n; i++) {
        remaining_bt[i] = plist[i].bt;
    }

    while (1) {
        int done = 1;
        for (int i = 0; i < n; i++) {
            if (remaining_bt[i] > 0) {
                done = 0;
                if (remaining_bt[i] > quantum) {
                    t += quantum;
                    remaining_bt[i] -= quantum;
                } else {
                    t += remaining_bt[i];
                    plist[i].wt = t - plist[i].bt;
                    remaining_bt[i] = 0;
                }
            }
        }
        if (done == 1) break;
    }
    free(remaining_bt);
}

// // Priority
// void findWaitingTimePriority(ProcessType plist[], int n) {
//     qsort(plist, n, sizeof(ProcessType), [] (const void *a, const void *b) {
//         return ((ProcessType *)a)->pri - ((ProcessType *)b)->pri;
//     });
//     findWaitingTimeFCFS(plist, n);
// }

// Turnaround time
void findTurnAroundTime(ProcessType plist[], int n) {
    for (int i = 0; i < n; i++) {
        plist[i].tat = plist[i].bt + plist[i].wt;
    }
}

// Average Time Calculations
void findavgTimeFCFS(ProcessType plist[], int n) {
    findWaitingTimeFCFS(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nFCFS\n");
}

void findavgTimeSJF(ProcessType plist[], int n) {
    findWaitingTimeSJF(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nSJF\n");
}

void findavgTimeRR(ProcessType plist[], int n, int quantum) {
    findWaitingTimeRR(plist, n, quantum);
    findTurnAroundTime(plist, n);
    printf("\n*********\nRR Quantum = %d\n", quantum);
}

void findavgTimePriority(ProcessType plist[], int n) {
    findWaitingTimePriority(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nPriority\n");
}

// Metrics Output
void printMetrics(ProcessType plist[], int n) {
    int total_wt = 0, total_tat = 0;
    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n");
    for (int i = 0; i < n; i++) {
        total_wt += plist[i].wt;
        total_tat += plist[i].tat;
        printf("\t%d\t\t%d\t\t%d\t\t%d\n", plist[i].pid, plist[i].bt, plist[i].wt, plist[i].tat);
    }
    printf("\nAverage waiting time = %.2f", (float)total_wt / n);
    printf("\nAverage turn around time = %.2f\n", (float)total_tat / n);
}

// Parsing Input
ProcessType* parse_file(FILE *input_file, int *n) {
    fscanf(input_file, "%d", n);
    ProcessType *plist = malloc((*n) * sizeof(ProcessType));
    for (int i = 0; i < *n; i++) {
        fscanf(input_file, "%d %d %d %d", &plist[i].pid, &plist[i].bt, &plist[i].pri, &plist[i].art);
        plist[i].wt = plist[i].tat = 0;
    }
    return plist;
}

// Driver Code
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
        return 1;
    }

    int n, quantum = 2;
    ProcessType *plist;
    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid file path\n");
        return 1;
    }

    plist = parse_file(input_file, &n);
    fclose(input_file);

    findavgTimeFCFS(plist, n);
    printMetrics(plist, n);

    findavgTimeSJF(plist, n);
    printMetrics(plist, n);

    findavgTimePriority(plist, n);
    printMetrics(plist, n);

    findavgTimeRR(plist, n, quantum);
    printMetrics(plist, n);

    free(plist);
    return 0;
}
