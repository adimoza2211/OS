#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

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
        exec_command(input);
    }
    return 0;
}