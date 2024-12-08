#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <grep argument>\n", argv[0]);
        exit(1);
    }

    char *grep_arg = argv[1]; // Argument passed to grep
    int pipefd1[2];           // Pipe between `cat` and `grep`
    int pipefd2[2];           // Pipe between `grep` and `sort`

    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", grep_arg, NULL};
    char *sort_args[] = {"sort", NULL};

    // Create first pipe
    if (pipe(pipefd1) == -1) {
        perror("Pipe 1 failed");
        exit(1);
    }

    // Fork first child (P1 -> cat scores)
    int pid1 = fork();
    if (pid1 < 0) {
        perror("Fork 1 failed");
        exit(1);
    }

    if (pid1 == 0) {
        // Child process P1 (executes cat scores)
        dup2(pipefd1[1], STDOUT_FILENO); // Redirect stdout to pipe1 write end
        close(pipefd1[0]);              // Close unused read end
        close(pipefd1[1]);              // Close write end after redirect
        execvp("cat", cat_args);        // Execute cat scores
        perror("execvp failed for cat"); // Exec only returns on error
        exit(1);
    }

    // Create second pipe
    if (pipe(pipefd2) == -1) {
        perror("Pipe 2 failed");
        exit(1);
    }

    // Fork second child (P2 -> grep <argument>)
    int pid2 = fork();
    if (pid2 < 0) {
        perror("Fork 2 failed");
        exit(1);
    }

    if (pid2 == 0) {
        // Child process P2 (executes grep <argument>)
        dup2(pipefd1[0], STDIN_FILENO);  // Redirect stdin to pipe1 read end
        dup2(pipefd2[1], STDOUT_FILENO); // Redirect stdout to pipe2 write end
        close(pipefd1[1]);               // Close unused write end of pipe1
        close(pipefd1[0]);               // Close read end of pipe1 after redirect
        close(pipefd2[0]);               // Close unused read end of pipe2
        close(pipefd2[1]);               // Close write end of pipe2 after redirect
        execvp("grep", grep_args);       // Execute grep <argument>
        perror("execvp failed for grep"); // Exec only returns on error
        exit(1);
    }

    // Fork third child (P3 -> sort)
    int pid3 = fork();
    if (pid3 < 0) {
        perror("Fork 3 failed");
        exit(1);
    }

    if (pid3 == 0) {
        // Child process P3 (executes sort)
        dup2(pipefd2[0], STDIN_FILENO); // Redirect stdin to pipe2 read end
        close(pipefd2[1]);             // Close unused write end of pipe2
        close(pipefd2[0]);             // Close read end of pipe2 after redirect
        execvp("sort", sort_args);     // Execute sort
        perror("execvp failed for sort"); // Exec only returns on error
        exit(1);
    }

    // Parent process closes all pipe ends
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);

    // Wait for all children to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);

    return 0;
}
