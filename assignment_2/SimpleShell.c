#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024


void exec_command(char *command)
{   

    pid_t pid;
    int status;
    pid = fork();
    time_t start_time;
    time(&start_time);
    pid_t pid_no= getpid();
    FILE *termination=fopen("termination.txt", "a");
    fprintf(termination, "%d     ", (int)pid_no);
    fprintf(termination, "%3f     ",(double)start_time);
    
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
        time_t end_time;
        time(&end_time);
        double difference = difftime(end_time, start_time);
        fprintf(termination, "%3f    \n", (double)difference);
        fclose(termination);
        exit(1);
    }
    else{
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        time_t end_time;
        time(&end_time);
        double difference = difftime(end_time, start_time);
        fprintf(termination, "%3f    \n", (double)difference);
        fclose(termination);
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

        FILE *termination=fopen("termination.txt", "a");
        if (pipe(pipefd) == -1) {
            perror("Pipe failure");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        time_t start_time;
        time(&start_time);
        pid_t pid_no=getpid();
        if (pid <0) {
            perror("Fork failure");
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
            fprintf(termination,"%s     ",pipes[i]);
            fprintf(termination, "%d    ", (int)pid_no);
            fprintf(termination, "%3f    ", (double)start_time);
            fclose(termination);
           
            
            token = strtok(pipes[i], " ");
            while (token != NULL) {
                args[j] = token;
                j++;
                token = strtok(NULL, " ");
            }
            args[j] = NULL;

            FILE *termination=fopen("termination.txt","a");
            time_t end_time;
            time(&end_time);
            double difference = difftime(end_time, start_time);
            fprintf(termination, "%3f    \n", (double)difference);
            fclose(termination);

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

bool isAnded(char* command)
{
    if(strchr(command,'&')!= NULL){
        return true;
    }
    else return false;
}

void exec_anded(char* command)
{
    char* args[100];
    int i = 0;
    char* token = strtok(command, "&");
    while(token != NULL)
    {
        args[i] = token;
        i++;
        FILE *termination= fopen("termination.txt", "a");
        fprintf(termination, "%s    ", token);
        fclose(termination);
        exec_command(token);
        token = strtok(NULL,"&");
    }

}

void view_history(){
    FILE *file = fopen("history.txt", "r");

    if (file == NULL) {
        perror("Error opening the file");
        exit(1);
    }

    // Read and print each line from the file
    char line[MAX_INPUT_SIZE]; // Adjust the buffer size as needed
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }

    // Close the file
    fclose(file);
}


void view_termination(){
    FILE *file = fopen("termination.txt", "r");

    if (file == NULL) {
        perror("Error opening the file");
        exit(1);
    }

    // Read and print each line from the file
    char line[MAX_INPUT_SIZE]; // Adjust the buffer size as needed
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }

    // Close the file
    fclose(file);

}


static void my_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nCaught SIGINT signal\n");
        view_termination();
        exit(1); // Exit the program after printing termination info
    }
}



int main()
{   
    FILE *history= fopen("history.txt", "w");
    fprintf(history, "%s","NO.      Command\n \n");
    fclose(history);
    int pc=0;
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_handler = my_handler;
    sigaction(SIGINT, &sig, NULL);
    
    FILE *termination= fopen("termination.txt", "w");
    fprintf(termination, "%s", "Command     Pid     Start Time      Duration:\n");
    fclose(termination);
    char input[MAX_INPUT_SIZE];
    while(1){
        printf("Group-48SimpleShell$ ");
        fgets(input,MAX_INPUT_SIZE,stdin);
        input[strlen(input)-1] = '\0';
        pc++;
        FILE *history= fopen("history.txt", "a");
        fprintf(history, "%d.    ", pc);
        fprintf(history, "%s    \n", input);
        fclose(history);
        if(strcmp(input,"history") == 0){
            view_history();
        }
        else{
            if(strcmp(input,"exit") == 0){
                printf("Exiting....\n");
                view_termination();
                break;
            }

            if(isPiped(input) == true){
                exec_piped(input);
            }
            else if(isAnded(input) == true){
                exec_anded(input);
            }
            else {
                FILE *termination= fopen("termination.txt", "a");
                fprintf(termination, "%s    ", input);
                fclose(termination);
                exec_command(input);
            }
            
        }
    }
    
    return 0;
}

//issues : duration is always zero . Ctrl C isnt working (program not entering my_handler) , add readme file, do documentations also... ig aur sab hogaya , 