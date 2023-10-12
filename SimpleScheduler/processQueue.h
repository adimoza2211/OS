#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#define MAX_JOBS 100
#define MAX_INPUT_SIZE 1024

struct Job{
    char name[MAX_INPUT_SIZE];
    pid_t pid;
    int waitTimeMillisecs;
    time_t startTime;
    time_t EndTime;
    int isRunnning;
};



// Define a simple queue for ready Jobs (you might want a more sophisticated data structure).
struct Queue {
    struct Job Jobs[100];
    int front;
    int rear;
};
struct Queue readyQueue;

void InitialiseQueue(struct Queue* q){
    q->front = q->rear = -1;
}

void enqueue(struct Queue* queue, struct Job Job) {
    if (queue->rear == 99) {
        printf("Queue is full.\n");
        return;
    }
    queue->Jobs[++queue->rear] = Job;
}

struct Job dequeue(struct Queue* queue) {
    if (queue->front == queue->rear) {
        struct Job dummy;
        return dummy; // Queue is empty
    }
    return queue->Jobs[++queue->front];
}

// Initialize the ready queue

// Define functions for Job management.
