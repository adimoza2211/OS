#include "SimpleShellHelper.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>



int main(int argc, char* argv[]){
    if(argc != 3){
        perror("NOT ENOUGH ARGUMENTS\n");
    }
    signal(SIGALRM, round_robin_schedule);
    InitialiseQueue(&readyQueue);
    ncpu = atoi(argv[1]);
    TSLICE = atoi(argv[2]);
    char input[MAX_INPUT_SIZE];
    while(1){
        printf("Group-48SimpleScheduler$ ");
        fgets(input,MAX_INPUT_SIZE,stdin);
        input[strlen(input)-1] = '\0';
        job_count++;
        exec_command(input);
    }



}