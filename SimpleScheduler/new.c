#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include "processQueue.h"
#define MAX_INPUT_SIZE 1024
#define NUM_PROCESSES 3 // Number of processes to run in round-robin

int NCPU;
int TIME_QUANTUM_US;


int current_process = 0;
pid_t child_pids[NUM_PROCESSES];

void child_process(char* cmd) {
    // Set the scheduling policy to SCHED_RR
    struct sched_param param;
    param.sched_priority = 1; // Adjust the priority as needed
    if (sched_setscheduler(0, SCHED_RR, &param) == -1) {
        perror("sched_setscheduler");
        exit(1);
    }

    // Execute the shell command
    if (execl("/bin/sh", "sh", "-c", cmd, NULL) == -1) {
        perror("execl");
        exit(1);
    }
}

void timer_handler(int signo) {
    
    // Handle timer expiration and switch to the next process
    if (kill(child_pids[current_process], SIGSTOP) == -1) {
        perror("kill");
        exit(1);
    }
    printf("executed SIGSTOP for %d TSLICE micro seconds", TIME_QUANTUM_US);

    // Move to the next process in the round-robin order
    current_process = (current_process + 1) % NUM_PROCESSES;

    // Send SIGCONT to the next process to resume it
    if (kill(child_pids[current_process], SIGCONT) == -1) {
        perror("kill");
        exit(1);
    }
}

void makeSubmit(char *str, size_t n) {
    size_t len = strlen(str);

    if (n >= len) {
        // If n is greater than or equal to the length of the string,
        // you effectively have an empty string.
        str[0] = '\0';
    } else {
        // Shift the characters in the string to remove the first n characters
        memmove(str, str + n, len - n + 1); // +1 to include the null-terminator
    }
}


int main(int argc, char* argv[]) {

    char input[MAX_INPUT_SIZE];
    int job_count = 0;
    if(argc != 3){
        perror("not enough arguments in new function");
    }
    

    // Set up timer
    struct sigaction sa;
    sa.sa_handler = timer_handler;
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = TIME_QUANTUM_US;
    timer.it_value = timer.it_interval;

    while (1) {
        printf("Group-48SimpleScheduler$ ");
        fgets(input, MAX_INPUT_SIZE, stdin);
        
        input[strlen(input)-1] = '\0';
        const char* s = "submit";
        char* found = strstr(input, s);
        if(found){
            makeSubmit(input, 7);
        }
        NCPU = atoi(argv[1]);
        TIME_QUANTUM_US = atoi(argv[2]);
        if (job_count < NUM_PROCESSES) {
            pid_t pid = fork();

            if (pid == -1) {
                perror("fork");
                return 1;
            }

            if (pid == 0) {
                child_process(input);
                return 0;
            } else {
                child_pids[job_count] = pid;
                job_count++;
            }
        } else {
            // All available processes are running, wait for one to finish
            pid_t current_pid = child_pids[current_process];
            
            // Set the timer for the current time quantum
            setitimer(ITIMER_REAL, &timer, NULL);
            
            // Wait for the current process to finish or for the timer to expire
            int status;
            waitpid(current_pid, &status, 0);
            
            // Disable the timer
            timer.it_value.tv_sec = 0;
            timer.it_value.tv_usec = 0;
            setitimer(ITIMER_REAL, &timer, NULL);

            // Move to the next process in the round-robin order
            current_process = (current_process + 1) % NUM_PROCESSES;

            // Launch the new job in place of the finished one
            pid_t new_pid = fork();

            if (new_pid == -1) {
                perror("fork");
                return 1;
            }

            if (new_pid == 0) {
                child_process(input);
                return 0;
            } else {
                child_pids[current_process] = new_pid;
            }
        }
    }
    //wait time = execution time/time quantum

    return 0;
}