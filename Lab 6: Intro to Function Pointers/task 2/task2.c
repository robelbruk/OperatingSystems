#include <stdio.h>
#include <stdlib.h>

/* Function declarations */
int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
int divide(int a, int b);
int exit_program(int a, int b);

int main(void)
{
    /* Predefined integers */
    int a = 10, b = 5;

    /* Array of function pointers */
    int (*operations[5])(int, int) = {add, subtract, multiply, divide, exit_program};

    /* Prompt user for input */
    char choice;
    printf("Enter a character ('0' for add, '1' for subtract, '2' for multiply, '3' for divide, '4' to exit): ");
    scanf(" %c", &choice);

    /* Convert character input to an integer index */
    int index = choice - '0';

    /* Execute the appropriate function */
    if (index >= 0 && index <= 4) {
        int result = operations[index](a, b);
        printf("Result: %d\n", result);
    } else {
        printf("Invalid input. Please enter a valid character.\n");
    }

    return 0;
}

/* Function definitions */
int add(int a, int b) {
    printf("Adding %d and %d\n", a, b);
    return a + b;
}

int subtract(int a, int b) {
    printf("Subtracting %d from %d\n", b, a);
    return a - b;
}

int multiply(int a, int b) {
    printf("Multiplying %d and %d\n", a, b);
    return a * b;
}

int divide(int a, int b) {
    if (b == 0) {
        printf("Error: Division by zero is undefined.\n");
        return 0;
    }
    printf("Dividing %d by %d\n", a, b);
    return a / b;
}

int exit_program(int a, int b) {
    printf("Exiting program.\n");
    exit(0);
}
