#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_INPUT_SIZE 1024

void exec_command(char *command)
{
    pid_t pid;
    int status;
    pid = fork();
    if (pid < 0)
    {
        printf("fork failed\n");
        exit(1);
    }
    else if (pid == 0)
    {
        char *args[100];
        int i = 0;
        char *token = strtok(command, " ");
        while (token != NULL)
        {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        execvp(args[0], args);
        perror("there was a failure in execvp");
        exit(1);
    }
    else{
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    }
}
bool isPiped(char* command)
{
    if(strchr(command,'|')!= NULL){
        return true;
    }
    else return false;
}
void exec_piped(char* command)
{
    int num_pipes = 0;
    char *pipes[10]; // Adjust the array size as needed

    // Tokenize the command string based on pipes
    char *token = strtok(command, "|");
    while (token != NULL) {
        pipes[num_pipes] = token;
        num_pipes++;
        token = strtok(NULL, "|");
    }

    int prev_pipe_read = -1; // File descriptor for the previous pipe's read end
    int pipefd[2];           // File descriptors for the current pipe

    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Child process
            // Close read end of the pipe
            close(pipefd[0]);

            // Redirect stdin if it's not the first command
            if (prev_pipe_read != -1) {
                dup2(prev_pipe_read, STDIN_FILENO);
                close(prev_pipe_read);
            }

            // Redirect stdout to the current pipe's write end
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);

            // Execute the command
            char *args[100]; // Adjust the array size as needed
            int j = 0;

            // Tokenize the individual command based on spaces
            token = strtok(pipes[i], " ");
            while (token != NULL) {
                args[j] = token;
                j++;
                token = strtok(NULL, " ");
            }
            args[j] = NULL;

            execvp(args[0], args);
            perror("execvp"); // Handle error if execvp fails
            exit(EXIT_FAILURE);
        } else { // Parent process
            // Close write end of the pipe
            close(pipefd[1]);

            // Close the previous pipe's read end
            if (prev_pipe_read != -1) {
                close(prev_pipe_read);
            }

            // Set the previous pipe's read end to the current pipe's read end
            prev_pipe_read = pipefd[0];
        }
    }

    // Read and print the output of the last command in the pipeline
    char output_buffer[1024]; // Adjust the buffer size as needed
    ssize_t num_bytes;

    while ((num_bytes = read(prev_pipe_read, output_buffer, sizeof(output_buffer))) > 0) {
        write(STDOUT_FILENO, output_buffer, num_bytes);
    }

    // Wait for the last child process to finish
    wait(NULL);
}
int main()
{
    char input[MAX_INPUT_SIZE];
    while(1){
        printf("Group-48SimpleShell$ ");
        fgets(input,MAX_INPUT_SIZE,stdin);
        input[strlen(input)-1] = '\0';
        if(strcmp(input,"exit") == 0){
            printf("Exiting....\n");
            break;
        }
        if(isPiped(input) == true){
            printf("Piped\n");
            exec_piped(input);
        }
        else exec_command(input);
        printf("Repeating\n");
    }
    return 0;
}