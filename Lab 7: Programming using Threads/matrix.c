#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20 // Define the matrix size

int matA[MAX][MAX]; 
int matB[MAX][MAX]; 
int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX]; 
int matProductResult[MAX][MAX]; 

// Structure to pass data to threads
typedef struct {
    int startRow;
    int endRow;
} ThreadData;

void fillMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void* computeSum(void* args) {
    ThreadData* data = (ThreadData*)args;
    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }
    pthread_exit(0);
}

void* computeDiff(void* args) {
    ThreadData* data = (ThreadData*)args;
    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }
    pthread_exit(0);
}

void* computeProduct(void* args) {
    ThreadData* data = (ThreadData*)args;
    for (int i = data->startRow; i < data->endRow; i++) {
        for (int j = 0; j < MAX; j++) {
            matProductResult[i][j] = 0;
            for (int k = 0; k < MAX; k++) {
                matProductResult[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
    pthread_exit(0);
}

int main() {
    srand(time(0)); // Seed random number generator

    // Fill the matrices with random values
    fillMatrix(matA);
    fillMatrix(matB);

    // Print the initial matrices
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    // Thread array
    pthread_t threads[10];
    ThreadData threadData[10];
    int rowsPerThread = MAX / 10;

    // Sum computation
    for (int i = 0; i < 10; i++) {
        threadData[i].startRow = i * rowsPerThread;
        threadData[i].endRow = (i + 1) * rowsPerThread;
        pthread_create(&threads[i], NULL, computeSum, &threadData[i]);
    }
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    // Difference computation
    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, computeDiff, &threadData[i]);
    }
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    // Product computation
    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, computeProduct, &threadData[i]);
    }
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print results
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);

    return 0;
}
