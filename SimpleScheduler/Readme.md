The SimpleScheduler starts from borrowing code from the SimpleShell which was the previous assignment
In this we use a custom submit command that takes in an executable, forks and sends to be made a child process
These child processes are inserted into the readyQueue
The readyQueue is a shared memory object that we have made of a Queue that has a struct Process as its field
Check processQueue.h for details on struct Process. It has many fields but the most important that we use in pid_t pid;
The scheduler is a daemon process that is made by calling the daemonise_scheduler() function that keeps it running in the background
We use semaphores to run NCPU number of processes from this queue. We dequeue the processes,
make use of the mutex fields in our shared memory structure by calling sem_wait(), run it for TSLICE seconds, make use of sem_post() and enqueue them back into process queue.
The scheduler runs for as long as the shell runs

Usage direction:
    call make new && ./new $NCPU $TSLICE(ms) note that $ indicates variable you have to enter integer values for both
    there will be a shell prompt in which you can enter the submit command
    submit $PRIORITY ./a.out & note that you have to enter priority before the name of the executable
    the shell will be immediately avaible after entering the commands since the process will run in the background
    keep entering the commands and they will keep getting scheduled
    press Ctrl C to get the wait times of individual processes in a table

Contributions:
Both members contributed equally in ideas and implementation in different parts
Modifying shell: Aditya
Coding priority queue: Aarya
Making shared memory: Aditya
Making semaphore logic: Aarya
SIGSTOP, SIGCONT logic: Aditya
handling signals: Aarya
enquee,dequeue of process: Aditya
wait time, execution time, scheduling: Aarya

Github repo link: https://github.com/adimoza2211/OS.git
