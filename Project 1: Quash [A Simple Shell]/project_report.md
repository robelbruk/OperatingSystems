# Shell Project Report: Quash


## Task Breakdown


### Task 1: Print the Shell Prompt and Add Built-in Commands
**Design Choices**: 
In Task 1, I began by setting up a basic shell structure to handle built-in commands. The shell prompt dynamically shows the current working directory by calling `getcwd()`, ensuring users see their location within the file system. The prompt syntax is `<current_path> >`. I implemented commands such as `cd`, `pwd`, `echo`, `exit`, `env`, and `setenv` to allow users to navigate and interact with their environment within the shell. Each built-in command was placed in its own function for modularity, improving readability and ease of future modifications.


### Task 2: Adding Process Forking
**Design Choices**: 
For Task 2, the shell’s ability to run non-built-in commands was implemented through process forking. If a command is not recognized as built-in, the shell forks a child process using `fork()`. The child process executes the command with `execvp()`, and if an error occurs, an error message is printed. The parent process waits for the child to complete using `waitpid()` unless the process is intended to run in the background (as handled in Task 3). This design isolates non-built-in commands in child processes, making the shell more stable.


### Task 3: Adding Background Processes
**Design Choices**: 
To implement background processes, I allowed users to append `&` to the end of a command. Upon detecting `&`, the shell runs the command in the background by forking the process and skipping `waitpid()`. A boolean flag tracks whether the command should run in the background. This feature frees the shell to return to the prompt immediately, accepting new commands while background processes execute independently. I also printed the process ID of each background task for easy tracking.


### Task 4: Signal Handling
**Design Choices**: 
The shell initially terminated upon receiving a `SIGINT` (Ctrl+C) signal, which was fixed in Task 4. I created a `SIGINT` handler that either forwards the signal to any active foreground process or simply redisplays the prompt if no such process exists. By ignoring `SIGINT` in the shell’s parent process, I allow users to terminate long-running or undesired commands without closing the shell itself, greatly improving usability.


### Task 5: Killing Long-Running Processes
**Design Choices**: 
In Task 5, a timer was added to terminate any foreground process running longer than 10 seconds. I used `SIGALRM` to handle this timeout, where a `SIGKILL` is sent to the process if it exceeds the limit. This is achieved by setting an alarm before a foreground process starts, which is then canceled if the process completes on time. This design choice safeguards users from accidentally running indefinitely long commands that could affect performance.


### Task 6: Implementing Output Redirection
**Design Choices**: 
For Task 6, I implemented output redirection using the `>` operator, allowing users to save command output to a file. Upon detecting `>`, the shell opens the specified file with write permissions and redirects `stdout` to this file using `dup2()`. This design isolates the redirection code within the external command function, ensuring that redirection is only applied when required. This feature expands the functionality of the shell, enabling file-based output while preserving the default shell behavior for other commands.


## Design Decisions


Several design choices were made to maintain simplicity and modularity. First, each built-in command is handled in a separate function, allowing for easy modification. The choice to fork child processes for non-built-in commands prevents the shell from crashing due to individual command failures. Background processes are handled with minimal impact on the main shell functionality, and the signal handlers are strategically designed to handle different types of signals (`SIGINT` and `SIGALRM`) without affecting other operations. Error handling in each feature ensures that users receive feedback for issues such as missing files or non-existent commands.


## Learnings


Through this project, I learned about the underlying mechanics of Unix-like command-line shells. Implementing process forking taught me about parent-child relationships between processes and how they interact with system resources. Signal handling, especially with `SIGINT` and `SIGALRM`, demonstrated how signals are used to manage inter-process communication and timing in an operating system environment. Finally, implementing features like background processes and output redirection provided insights into concurrent programming, where multiple tasks are managed effectively within a single application interface.