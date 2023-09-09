#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024

void exec_command(char *command)
{
    pid_t pid;  // the program descriptor pid is created 
    int status;
    pid = fork();  //Process is forked to give : child and parent process
    if (pid < 0)
    {
        printf("fork failed\n");  //Function exits if fork failed
        exit(1);
    }
    else if (pid == 0) // when child process is executed
    {
        char *args[100];
        int i = 0;
        char *token = strtok(command, " "); //breaks the command string into substrings with seperator as ' '. 
        while (token != NULL)
        {
            args[i] = token;  
            i++;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;
        execvp(args[0], args);  //The input command is run by using execvp.
        perror("there was a failure in execvp");   // execvp has failed and function is exited.
        exit(1);
    }
    else{
        // the parent waits for the child with specified pid to be terminated 
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));  //it checks if the child has terminated normally or has returned from the kernel (interrupt raised).

    }
}
int main()
{
    char input[MAX_INPUT_SIZE];
    // Infinite while loop for shell is created.
    while(1){
        printf("Group-48SimpleShell$ ");
        fgets(input,MAX_INPUT_SIZE,stdin);
        input[strlen(input)-1] = '\0';
        if(strcmp(input,"exit") == 0){   // if input string is "exit" the shell exits.
            printf("Exiting....\n");
            break;
        }
        exec_command(input);  // input string is executed using the exec_command function.
    }
    return 0;
}
