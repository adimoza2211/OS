#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
volatile int FirstisRunning = 0;
volatile int SecondisRunning = 0;

void sigvta_handler(int signum){
    printf("Signal recieved\n");

}



int main(int argc, char *argv[])
{   
    signal(SIGVTALRM, sigvta_handler);
    if(argc != 2){
        return -1;
    }
    char* fileToExec = argv[1];
    int pid = fork();
    int status;
    if(pid < 0){
        printf("fork failed\n");
        exit(1);
    }   
    else if(pid == 0 && FirstisRunning == 0 && SecondisRunning == 0){ //for successful, child created
        //it shouldn't simply execute in this case, it should wait for a signal from the scheduler
        //since we will be using a round robin policy to schedule this tasks
        
        if(execvp(fileToExec, argv) == -1){
            perror("execvp");
            return 1;
        }
    }
    else{
        do {
            waitpid(pid, &status, WUNTRACED);   //waits for the child to complete execution and terminate
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 0;
}
