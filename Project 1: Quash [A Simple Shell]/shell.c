#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

extern char **environ; // Access to global environment variables
char prompt[] = "> ";
char delimiters[] = " \t\r\n";

// Flag to track foreground processes
volatile sig_atomic_t child_pid = 0;

void print_prompt() {
    char cwd[MAX_COMMAND_LINE_LEN];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s%s ", cwd, prompt);
    } else {
        perror("getcwd");
    }
}

// Signal handler for SIGINT (Ctrl+C)
void handle_sigint(int signum) {
    if (child_pid > 0) {
        // Send SIGINT to the foreground process (child)
        kill(child_pid, SIGINT);
    } else {
        printf("\n"); // Just print a newline for the prompt
        print_prompt(); // Redisplay prompt after Ctrl+C
        fflush(stdout);
    }
}

// Signal handler for SIGALRM to terminate long-running processes
void handle_sigalrm(int signum) {
    if (child_pid > 0) {
        printf("\nProcess %d exceeded time limit and was terminated\n", child_pid);
        kill(child_pid, SIGKILL); // Forcefully terminate the process
    }
}

void execute_cd(char *path) {
    if (path == NULL || strcmp(path, "~") == 0) {
        path = getenv("HOME"); // Default to home directory
    }
    if (chdir(path) != 0) {
        perror("cd failed");
    }
}

void execute_pwd() {
    char cwd[MAX_COMMAND_LINE_LEN];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd failed");
    }
}

void execute_echo(char **args) {
    int i;
    for (i = 1; args[i] != NULL; i++) {
        if (args[i][0] == '$') {
            char *env_var = getenv(args[i] + 1); // Get value without the '$'
            if (env_var) {
                printf("%s ", env_var);
            }
        } else {
            printf("%s ", args[i]);
        }
    }
    printf("\n");
}

void execute_env() {
    char **env;
    for (env = environ; *env != NULL; env++) {
        printf("%s\n", *env);
    }
}

void execute_setenv(char *name, char *value) {
    if (name && value) {
        if (setenv(name, value, 1) != 0) {
            perror("setenv failed");
        }
    } else {
        fprintf(stderr, "Usage: setenv VAR VALUE\n");
    }
}

void execute_external_command(char **args, bool background) {
    int fd = -1; // File descriptor for redirection
    bool redirect_output = false;
    char *output_file = NULL;

    // Check for output redirection using '>'
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            redirect_output = true;
            output_file = args[i + 1]; // The file after '>'
            args[i] = NULL; // Terminate args at '>'
            break;
        }
    }

    pid_t pid = fork();
    if (pid == 0) {
        // In child process, restore default SIGINT and SIGALRM behavior
        signal(SIGINT, SIG_DFL);
        signal(SIGALRM, SIG_DFL);

        // Handle output redirection
        if (redirect_output && output_file != NULL) {
            fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Failed to open file for output redirection");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO); // Redirect stdout to the file
            close(fd);
        }

        // Execute the command
        if (execvp(args[0], args) == -1) {
            perror("execvp() failed");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        // Fork failed
        perror("Fork failed");
    } else {
        // In parent process
        if (background) {
            printf("Process %d running in background\n", pid);
        } else {
            // Set global child_pid for foreground process
            child_pid = pid;

            // Start a 10-second timer for the foreground process
            alarm(10);

            int status;
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid failed");
            }

            // Reset the timer if process completes before 10 seconds
            alarm(0);
            child_pid = 0; // Reset child_pid after process finishes

            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                fprintf(stderr, "An error occurred.\n");
            }
        }
    }
}

int main() {
    // Register signal handlers
    signal(SIGINT, handle_sigint);
    signal(SIGALRM, handle_sigalrm);

    char command_line[MAX_COMMAND_LINE_LEN];
    char *arguments[MAX_COMMAND_LINE_ARGS];

    while (true) {
        // Print the shell prompt
        print_prompt();
        fflush(stdout);

        // Read input from stdin
        if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
            fprintf(stderr, "fgets error");
            exit(0);
        }

        if (feof(stdin)) { // Exit on EOF (Ctrl+D)
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        // Remove newline character from the end of command
        command_line[strlen(command_line) - 1] = '\0';

        // Tokenize the command line input
        int arg_count = 0;
        char *token = strtok(command_line, delimiters);
        while (token != NULL && arg_count < MAX_COMMAND_LINE_ARGS - 1) {
            arguments[arg_count++] = token;
            token = strtok(NULL, delimiters);
        }
        arguments[arg_count] = NULL; // Null-terminate the array of arguments

        // Check for background execution '&' symbol
        bool background = false;
        if (arg_count > 0 && strcmp(arguments[arg_count - 1], "&") == 0) {
            background = true;
            arguments[arg_count - 1] = NULL; // Remove '&' from arguments
        }

        // Check for built-in commands
        if (arg_count == 0) {
            continue; // No command entered
        } else if (strcmp(arguments[0], "cd") == 0) {
            execute_cd(arguments[1]);
        } else if (strcmp(arguments[0], "pwd") == 0) {
            execute_pwd();
        } else if (strcmp(arguments[0], "echo") == 0) {
            execute_echo(arguments);
        } else if (strcmp(arguments[0], "exit") == 0) {
            exit(0);
        } else if (strcmp(arguments[0], "env") == 0) {
            execute_env();
        } else if (strcmp(arguments[0], "setenv") == 0) {
            execute_setenv(arguments[1], arguments[2]);
        } else {
            // Execute external command with background and redirection support
            execute_external_command(arguments, background);
        }
    }
    return 0;
}
