#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int fd1[2]; // Pipe 1: Parent to Child
    int fd2[2]; // Pipe 2: Child to Parent

    char fixed_str1[] = "howard.edu";
    char fixed_str2[] = "gobison.org";
    char input_str[100];
    char second_input_str[100];
    pid_t p;

    // Create pipes
    if (pipe(fd1) == -1) {
        fprintf(stderr, "Pipe 1 Failed");
        return 1;
    }
    if (pipe(fd2) == -1) {
        fprintf(stderr, "Pipe 2 Failed");
        return 1;
    }

    printf("Enter a string to concatenate: ");
    scanf("%s", input_str);
    p = fork();

    if (p < 0) {
        fprintf(stderr, "fork Failed");
        return 1;
    }

    // Parent process
    else if (p > 0) {
        close(fd1[0]); // Close reading end of Pipe 1
        close(fd2[1]); // Close writing end of Pipe 2

        // Write input string to Pipe 1
        write(fd1[1], input_str, strlen(input_str) + 1);
        close(fd1[1]); // Close writing end of Pipe 1 after writing

        // Wait for child process to finish
        wait(NULL);

        // Read concatenated string from Pipe 2
        char result_str[200];
        read(fd2[0], result_str, 200);
        printf("Output from parent: %s\n", result_str);

        close(fd2[0]); // Close reading end of Pipe 2
    }

    // Child process
    else {
        close(fd1[1]); // Close writing end of Pipe 1
        close(fd2[0]); // Close reading end of Pipe 2

        // Read string from Pipe 1
        char concat_str1[200];
        read(fd1[0], concat_str1, 200);
        close(fd1[0]); // Close reading end of Pipe 1 after reading

        // Concatenate with "howard.edu"
        int k = strlen(concat_str1);
        for (int i = 0; i < strlen(fixed_str1); i++) {
            concat_str1[k++] = fixed_str1[i];
        }
        concat_str1[k] = '\0'; // Null-terminate the string

        printf("Output from child: %s\n", concat_str1);

        // Send concatenated string back to parent via Pipe 2
        write(fd2[1], concat_str1, strlen(concat_str1) + 1);

        // Prompt user for a second input
        printf("Enter another string to concatenate: ");
        scanf("%s", second_input_str);

        // Concatenate with "gobison.org"
        int m = strlen(second_input_str);
        for (int i = 0; i < strlen(fixed_str2); i++) {
            second_input_str[m++] = fixed_str2[i];
        }
        second_input_str[m] = '\0'; // Null-terminate the string

        printf("Output from child after second input: %s\n", second_input_str);

        exit(0); // Terminate child process
    }
    return 0;
}
