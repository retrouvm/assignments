#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>  // For errno, ETIMEDOUT
#include <string.h> // For strncmp
#include <time.h>   // For clock_gettime

// Define PCB (Process Control Block)
typedef struct {
    int pid;
    int priority;
    int cpuIndex;
    int ioIndex;
    int *cpuBurst;
    int *ioBurst;
    int numCPUBurst;
    double arrivalTime;
    double completionTime;
} PCB;

// Define a node in doubly linked list
typedef struct Node {
    PCB *pcb;
    struct Node *prev;
    struct Node *next;
} Node;

Node *readyQueue = NULL;  // head of readyQueue
Node *ioQueue = NULL;  // head of ioQueue

// Semaphores
sem_t sem_cpu;
sem_t sem_io;

// Flags to indicate if file read and schedulers are done
int file_read_done = 0;
int cpu_sch_done = 0;
int io_sys_done = 0;
int cpu_busy = 0;
int io_busy = 0;

double totalCpuTime = 0; // in milliseconds
double totalElapsedTime = 0; // in milliseconds
int totalCompletedProcesses = 0;
double totalTurnaroundTime = 0; // in milliseconds
double totalWaitingTime = 0; // in milliseconds

FILE *outputFile; 
// Function to add a node to the end of a doubly linked list
void addNode(Node **head, PCB *pcb) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->pcb = pcb;
    newNode->prev = NULL;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        Node *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
        newNode->prev = temp;
    }
}

Node* removeHeadNode(Node **head) {
    if (*head == NULL) return NULL;

    Node *temp = *head;
    *head = (*head)->next;

    if (*head != NULL) {
        (*head)->prev = NULL;
    }

    return temp;
}

// FileRead Thread
void *FileRead_thread(void *arg) {
    char *fileName = (char *)arg;
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Unable to open file");
        return NULL;
    }
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "proc", 4) == 0) {
            // Create a PCB
            PCB *newPCB = (PCB *)malloc(sizeof(PCB));
            if (newPCB == NULL) {
                perror("Memory allocation failed");
                return NULL;
            }
            
            // Variables to hold CPU and I/O bursts
            int cpuBursts[10] = {0};
            int ioBursts[10] = {0};
            int numFields;
            
            // Parse the input line
            numFields = sscanf(line, "proc %d %d %d %d %d %d %d %d %d %d %d",
                            &newPCB->pid,
                            &newPCB->priority,
                            cpuBursts, cpuBursts + 1, cpuBursts + 2, cpuBursts + 3, cpuBursts + 4,
                            ioBursts, ioBursts + 1, ioBursts + 2, ioBursts + 3);
            
            // Assign values
            // Assign values
            newPCB->numCPUBurst = (numFields - 2) / 2;  // Missing in your code
            newPCB->cpuBurst = malloc(newPCB->numCPUBurst * sizeof(int));
            newPCB->ioBurst = malloc((newPCB->numCPUBurst - 1) * sizeof(int));

            if (newPCB->cpuBurst == NULL || newPCB->ioBurst == NULL) {
                perror("Memory allocation failed");
                return NULL;
            }
            
            // Initialize other PCB fields
            newPCB->cpuIndex = 0;
            newPCB->ioIndex = 0;
            
            // Capture the arrival time
            struct timespec arrivalTimeSpec;
            clock_gettime(CLOCK_REALTIME, &arrivalTimeSpec);
            newPCB->arrivalTime = arrivalTimeSpec.tv_sec * 1000.0 + arrivalTimeSpec.tv_nsec / 1000000.0;
            
            // Add the new PCB to the readyQueue
            addNode(&readyQueue, newPCB);
            
            sem_post(&sem_cpu);
            fclose(file);
            return NULL;
        } else if (strncmp(line, "sleep", 5) == 0) {
            int sleep_time;
            sscanf(line, "sleep %d", &sleep_time);
            usleep(sleep_time * 1000);
            fclose(file);
            return NULL;
        } else if (strncmp(line, "stop", 4) == 0) {
            break;
            fclose(file);
            return NULL;
        }
    }

    fclose(file);
    file_read_done = 1;
    return NULL;
}


// CPU Scheduler Thread (FIFO)
void* cpuScheduler_thread(void *arg) {
    struct timespec atimespec;
    atimespec.tv_sec = 1; // 1 second
    atimespec.tv_nsec = 0; // 0 nanoseconds

    while(1) {
        if(readyQueue == NULL && cpu_busy == 0 && ioQueue == NULL && io_busy == 0 && file_read_done == 1) {
            break;
        }

        int res = sem_timedwait(&sem_cpu, &atimespec);
        if(res == -1 && errno == ETIMEDOUT) continue;

        // Assuming cpu_busy flag, set it to indicate CPU is busy
        cpu_busy = 1;

        // Get (remove) the first PCB from Ready_Q
        Node *node = removeHeadNode(&readyQueue);
        if (node == NULL) {
            cpu_busy = 0;
            continue;
        }
        PCB *currentPCB = node->pcb;
        free(node);  // Free the node
        // Capture the time the process starts executing
        struct timespec startTimeSpec;
        clock_gettime(CLOCK_REALTIME, &startTimeSpec);
        // double startTime = startTimeSpec.tv_sec * 1000.0 + startTimeSpec.tv_nsec / 1000000.0;
        
        // Sleep for the CPU burst time
        usleep(currentPCB->cpuBurst[currentPCB->cpuIndex] * 1000); // convert to microseconds

        // Update the CPU time used
        totalCpuTime += currentPCB->cpuBurst[currentPCB->cpuIndex];
        
        // Update CPU index for next time
        currentPCB->cpuIndex++;

        if(currentPCB->cpuIndex >= currentPCB->numCPUBurst) { // This is the last CPU burst
            // Capture the time the process has completed
            struct timespec endTimeSpec;
            clock_gettime(CLOCK_REALTIME, &endTimeSpec);
            double endTime = endTimeSpec.tv_sec * 1000.0 + endTimeSpec.tv_nsec / 1000000.0;

            // Update metrics
            totalCompletedProcesses++;
            currentPCB->completionTime = endTime;
            totalTurnaroundTime += (endTime - currentPCB->arrivalTime);
            totalWaitingTime += (endTime - currentPCB->arrivalTime) - totalCpuTime;

            // Terminate this PCB
            free(currentPCB);
            
            // Assuming cpu_busy flag, set it to indicate CPU is free
            cpu_busy = 0;
        } else {
            // Insert PCB into IO_Q
            addNode(&ioQueue, currentPCB);
            sem_post(&sem_io);
        }
    }
    // Flag to indicate CPU scheduler thread is done
    cpu_sch_done = 1;
    return NULL;
}


// I/O System Thread
void *IOSystem_thread(void *arg) {
    while (1) {
        if (readyQueue == NULL && !cpu_busy && ioQueue == NULL && !io_busy && file_read_done) {
            break;
        }

        int res = sem_wait(&sem_io);  // Wait until a new PCB is available in ioQueue
        if (res == -1) continue;

        io_busy = 1;

        Node *node = removeHeadNode(&ioQueue);
        if (node == NULL) {
            io_busy = 0;
            continue;
        }
        PCB *pcb = node->pcb;
        free(node);  // Free the node

        // Insert back to readyQueue for next CPU burst
        addNode(&readyQueue, pcb);
        io_busy = 0;
        sem_post(&sem_cpu);
    }

    io_sys_done = 1;
    return NULL;
}

void calculate_and_print_metrics(double totalElapsedTime) {
    double cpuUtilization = (totalCpuTime / totalElapsedTime) * 100;
    double throughput = totalCompletedProcesses / (totalElapsedTime / 1000); // processes per second
    double avgTurnaroundTime = totalTurnaroundTime / totalCompletedProcesses;
    double avgWaitingTime = totalWaitingTime / totalCompletedProcesses;

    // Write metrics to output.txt
    fprintf(outputFile, "\nPerformance Metrics:\n");
    fprintf(outputFile, "-----------------------------------------------------------------\n");
    fprintf(outputFile, "CPU Utilization          : %.2f%%\n", cpuUtilization);
    fprintf(outputFile, "Throughput               : %.2f processes/sec\n", throughput);
    fprintf(outputFile, "Average Turnaround Time  : %.2f ms\n", avgTurnaroundTime);
    fprintf(outputFile, "Average Waiting Time     : %.2f ms\n", avgWaitingTime);
    fprintf(outputFile, "-----------------------------------------------------------------\n");
}


int main(int argc, char *argv[]) {
    char *algorithm = NULL;
    char *inputFile = NULL;

    outputFile = fopen("output.txt", "w");
    if(outputFile == NULL) {
        perror("Error opening output.txt");
        fclose(outputFile);
        return 0;
    }
    // Parse command-line arguments
    for (int i = 1; i < argc; i += 2) { // Increment by 2 to skip to the next flag
        if (strcmp(argv[i], "-alg") == 0) {
            algorithm = argv[i + 1];
        } else if (strcmp(argv[i], "-input") == 0) {
            inputFile = argv[i + 1];
        } else {
            fprintf(stderr, "Unknown command-line option: %s\n", argv[i]);
            return 1;
        }
    }

    // Check if arguments are provided
    if (algorithm == NULL || inputFile == NULL) {
        fprintf(stderr, "Missing required command-line arguments\n");
        fclose(outputFile);
        return 0;
    }
    // Create and initialize semaphores, variables, and data structures
    sem_init(&sem_cpu, 0, 0);
    sem_init(&sem_io, 0, 0);

    // Capture the start time before starting the threads
    struct timespec startTimeSpec;
    clock_gettime(CLOCK_REALTIME, &startTimeSpec);
    double startTime = startTimeSpec.tv_sec * 1000.0 + startTimeSpec.tv_nsec / 1000000.0;

    // Start FileRead_thread
    pthread_t fileRead_thread;
    pthread_create(&fileRead_thread, NULL, FileRead_thread, (void *)"input.txt");

    pthread_t cpuScheduler_thread_var;
    pthread_create(&cpuScheduler_thread_var, NULL, cpuScheduler_thread, NULL); // Corrected the name here

    pthread_t IOSystem_thread_var;
    pthread_create(&IOSystem_thread_var, NULL, IOSystem_thread, NULL); // Corrected the name here




    // Wait for threads to finish
    pthread_join(fileRead_thread, NULL);
    pthread_join(cpuScheduler_thread_var, NULL);
    pthread_join(IOSystem_thread_var, NULL);

    struct timespec endTimeSpec;
    clock_gettime(CLOCK_REALTIME, &endTimeSpec);
    double endTime = endTimeSpec.tv_sec * 1000.0 + endTimeSpec.tv_nsec / 1000000.0;
    // Now you can find total elapsed time.
    totalElapsedTime = endTime - startTime;
    // Destroy semaphores
    sem_destroy(&sem_cpu);
    sem_destroy(&sem_io);

    // Calculate and print performance metrics
    calculate_and_print_metrics(totalElapsedTime);
    fclose(outputFile);
    return 0;
}
