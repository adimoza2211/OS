#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#define NCPU 2             // Total number of CPU resources
#define TSLICE 1000        // Time quantum in milliseconds
#define MAX_COMMAND_LEN 50 // Maximum command length

// Global variables
pid_t running_processes[NCPU];
int num_running = 0;

void scheduler() {
    while (num_running > 0) {
        for (int i = 0; i < num_running; i++) {
            // Simulate a time quantum
            usleep(TSLICE * 1000); // usleep takes microseconds, so we multiply by 1000

            // Send SIGSTOP to pause the process
            kill(running_processes[i], SIGSTOP);
        }
    }
}

void submit(const char *executable) {
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process: execute the provided executable
        execl(executable, executable, (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        // Parent process: add the child to the running_processes array
        if (num_running < NCPU) {
            running_processes[num_running] = pid;
            num_running++;
            kill(pid, SIGSTOP); // Pause the process
        } else {
            printf("No available CPU resources for %s\n", executable);
        }
    }
}

int main() {
    char command[MAX_COMMAND_LEN];

    // Start the scheduler in a separate process
    pid_t scheduler_pid = fork();

    if (scheduler_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (scheduler_pid == 0) {
        // Child process becomes the scheduler
        scheduler();
    } else {
        // Parent process serves as the shell
        while (1) {
            printf("SimpleShell$ ");
            if (fgets(command, sizeof(command), stdin) == NULL) {
                perror("fgets");
                break;
            }

            // Remove newline character
            char *newline = strchr(command, '\n');
            if (newline) {
                *newline = '\0';
            }

            if (strcmp(command, "exit") == 0) {
                // Terminate the scheduler when exiting the shell
                kill(scheduler_pid, SIGTERM);
                break;
            } else if (strncmp(command, "submit ", 7) == 0) {
                const char *executable = command + 7;
                submit(executable);
            }
        }

        // Wait for all child processes to terminate
        while (num_running > 0) {
            int status;
            pid_t terminated_pid = wait(&status);
            if (terminated_pid == -1) {
                perror("wait");
            } else {
                // Remove the terminated process from the running_processes array
                for (int i = 0; i < num_running; i++) {
                    if (running_processes[i] == terminated_pid) {
                        for (int j = i; j < num_running - 1; j++) {
                            running_processes[j] = running_processes[j + 1];
                        }
                        num_running--;
                        break;
                    }
                }
            }
        }
    }

    return 0;
}
