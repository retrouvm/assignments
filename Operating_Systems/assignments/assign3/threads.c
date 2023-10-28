#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>


// Process Control Block
typedef struct PCB {
    int pid;
    int priority;
    int processID;
    int *CPUBurst;
    int *IOBurst;
    int numCPUBursts;
    int numIOBursts;
    int cpuIndex;
    int ioIndex;
    struct PCB *next;
    struct PCB *prev;
} PCB;

// Double-linked list
typedef struct {
    PCB *head;
    PCB *tail;
} DLL;

//Program arguments
typedef struct {
    char *inputFileName;
    char *algorithm;
    int quantum;
} ProgramArgs;


// Functions to manipulate DLL
void insert(DLL *list, PCB *pcb);
PCB* pop(DLL *list);

// Global Variables
DLL Ready_Q, IO_Q;
int file_read_done = 0, cpu_sch_done = 0, io_sys_done = 0, cpu_busy = 0, io_busy = 0;
sem_t sem_cpu, sem_io;
pthread_mutex_t mutex_ready_q;
pthread_mutex_t mutex_io_q;


//DLL functions
void insert(DLL *list, PCB *pcb) {
    pcb->next = NULL;
    pcb->prev = list->tail;
    if (list->tail)
        list->tail->next = pcb;
    else
        list->head = pcb;
    list->tail = pcb;
}

PCB* pop(DLL *list) {
    if (!list->head) return NULL;
    PCB *pcb = list->head;
    list->head = list->head->next;
    if (list->head)
        list->head->prev = NULL;
    else
        list->tail = NULL;
    return pcb;
}

PCB* findShortestJob(DLL *list) {
    if (!list->head) return NULL; // If list is empty

    PCB *current = list->head;
    PCB *shortestJob = current;

    // Iterate through the list and find the shortest job
    while (current) {
        if (current->CPUBurst[current->cpuIndex] < shortestJob->CPUBurst[shortestJob->cpuIndex]) {
            shortestJob = current;
        }
        current = current->next;
    }

    // Remove shortestJob from the list
    if (shortestJob->prev) {
        shortestJob->prev->next = shortestJob->next;
    } else {
        list->head = shortestJob->next;
    }

    if (shortestJob->next) {
        shortestJob->next->prev = shortestJob->prev;
    } else {
        list->tail = shortestJob->prev;
    }

    shortestJob->next = NULL;
    shortestJob->prev = NULL;

    return shortestJob;
}


PCB* findHighestPriority(DLL *list) {
    if (!list->head) return NULL; // If list is empty

    PCB *current = list->head;
    PCB *highestPriority = current;

    // Iterate through the list and find the highest priority job
    while (current) {
        if (current->priority < highestPriority->priority) {  // Lower numbers indicate higher priority
            highestPriority = current;
        }
        current = current->next;
    }

    // Remove highestPriority from the list
    if (highestPriority->prev) {
        highestPriority->prev->next = highestPriority->next;
    } else {
        list->head = highestPriority->next;
    }

    if (highestPriority->next) {
        highestPriority->next->prev = highestPriority->prev;
    } else {
        list->tail = highestPriority->prev;
    }

    highestPriority->next = NULL;
    highestPriority->prev = NULL;

    return highestPriority;
}
PCB* createPCB(const char* line) {
    PCB* newPCB = (PCB*)malloc(sizeof(PCB));
    if (newPCB == NULL) {
        perror("Failed to allocate memory for PCB");
        exit(EXIT_FAILURE);
    }

    // Start by checking if the line represents a 'proc'
    if (strncmp(line, "proc", 4) != 0) {
        // This isn't a process definition line.
        free(newPCB);
        return NULL;
    }

    // Extract the process ID and the number of bursts
    int offset = 0;
    int numRead = sscanf(line, "proc %d %d%n", &(newPCB->processID), &(newPCB->numCPUBursts), &offset);
    if (numRead != 2) {
        perror("Failed to parse processID or numCPUBursts");
        free(newPCB);
        exit(EXIT_FAILURE);
    }

    // Given the number of bursts, we know that half (or half + 1 for odd numbers) will be CPU bursts
    int numCPUBursts = (newPCB->numCPUBursts + 1) / 2;
    int numIOBursts = newPCB->numCPUBursts / 2;

    newPCB->CPUBurst = (int*)malloc(numCPUBursts * sizeof(int));
    newPCB->IOBurst = (int*)malloc(numIOBursts * sizeof(int));
    if (newPCB->CPUBurst == NULL || newPCB->IOBurst == NULL) {
        perror("Failed to allocate memory for bursts");
        free(newPCB->CPUBurst);
        free(newPCB->IOBurst);
        free(newPCB);
        exit(EXIT_FAILURE);
    }

    // Read bursts alternating between CPU and IO
    const char* ptr = line + offset;
    for (int i = 0; i < newPCB->numCPUBursts; i++) {
        if (i % 2 == 0) {  // CPU Burst
            sscanf(ptr, "%d%n", &(newPCB->CPUBurst[i / 2]), &offset);
        } else {  // IO Burst
            sscanf(ptr, "%d%n", &(newPCB->IOBurst[i / 2]), &offset);
        }
        ptr += offset;
    }

    return newPCB;
}


// To free the memory when done:
void destroyPCB(PCB* pcb) {
    if(pcb != NULL) {
        if(pcb->CPUBurst != NULL) {
            free(pcb->CPUBurst);
        }
        if(pcb->IOBurst != NULL) {
            free(pcb->IOBurst);
        }
        free(pcb);
    }
}

//threads
void* FileRead_thread(void* arg) {
    const char* filename = (const char*) arg;
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Trim newline character
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }

        // Stop check
        if (strncmp(line, "stop", 4) == 0) {
            break;
        }
        
        // Sleep check
        if (strncmp(line, "sleep", 5) == 0) {
            // Assuming the format is "sleep [duration]", you can extract the duration and sleep for that amount
            int sleepDuration = 0;
            sscanf(line, "sleep %d", &sleepDuration);
            sleep(sleepDuration); // This will cause the thread to sleep
            continue;
        }

        PCB *newPCB = createPCB(line); 
        if (!newPCB) {
            continue;
        }

        pthread_mutex_lock(&mutex_ready_q);
        insert(&Ready_Q, newPCB);
        pthread_mutex_unlock(&mutex_ready_q);
        
        sem_post(&sem_cpu);
    }
    
    // If you want to perform an action upon EOF or if other threads need to know when reading is done:
    // pthread_mutex_lock(&some_mutex);
    file_read_done = 1;
    // pthread_mutex_unlock(&some_mutex);

    fclose(file);
    return NULL;
}



void* CPUscheduler_thread(void *arg) {
    ProgramArgs *args = (ProgramArgs *) arg;
    while (1) {
        if (Ready_Q.head == NULL && !cpu_busy && IO_Q.head == NULL && !io_busy && file_read_done) break;

        sem_wait(&sem_cpu);
        cpu_busy = 1;
        PCB *pcb = NULL;
        pthread_mutex_lock(&mutex_ready_q);
        if (strcmp(args->algorithm, "FIFO") == 0) {
            pcb = pop(&Ready_Q);
        } else if (strcmp(args->algorithm, "SJF") == 0) {
            pcb = findShortestJob(&Ready_Q);
        } else if (strcmp(args->algorithm, "PR") == 0) {
            pcb = findHighestPriority(&Ready_Q);
        } else if (strcmp(args->algorithm, "RR") == 0) {
            pcb = pop(&Ready_Q);
            if (pcb->CPUBurst[pcb->cpuIndex] > args->quantum) {
                usleep(args->quantum * 1000);  // Assuming quantum is in ms
                pcb->CPUBurst[pcb->cpuIndex] -= args->quantum;
                insert(&Ready_Q, pcb);
                cpu_busy = 0;
                pthread_mutex_unlock(&mutex_ready_q); // Make sure to unlock here before continue
                continue;
            } else if (pcb->CPUBurst[pcb->cpuIndex] == args->quantum) {
                usleep(args->quantum * 1000);
                pcb->cpuIndex++;
            }
        }

        if (!pcb) {
            cpu_busy = 0;
            pthread_mutex_unlock(&mutex_ready_q); // Make sure to unlock here before continue
            continue;
        }
        // Unlock the mutex after you're done with the Ready_Q
        pthread_mutex_unlock(&mutex_ready_q);

        if (strcmp(args->algorithm, "RR") != 0 || (strcmp(args->algorithm, "RR") == 0 && pcb->CPUBurst[pcb->cpuIndex-1] < args->quantum)) {
            usleep(pcb->CPUBurst[pcb->cpuIndex] * 1000); // Assuming CPUBurst is in ms
            pcb->cpuIndex++;
        }

        if (pcb->cpuIndex >= pcb->numCPUBursts) {
            free(pcb->CPUBurst);
            free(pcb->IOBurst);
            free(pcb);
            cpu_busy = 0;
        } else {
            insert(&IO_Q, pcb);
            sem_post(&sem_io);
            cpu_busy = 0;
        }
    }

    cpu_sch_done = 1;
    return NULL;
}



void* IO_system_thread(void *arg) {
    while (1) {  // change this to always loop
        // Wait for an IO task, this will block if IO_Q is empty
        sem_wait(&sem_io);
        io_busy = 1;

        pthread_mutex_lock(&mutex_io_q);

        // If IO_Q is empty and CPU scheduler is done, break out of loop
        if (IO_Q.head == NULL && cpu_sch_done) {
            pthread_mutex_unlock(&mutex_io_q);
            io_busy = 0;
            break;  // This will exit the loop and the thread function
        }
        
        // Just in case of a spurious wake-up or CPU scheduler is done but IO_Q is not empty
        if (IO_Q.head == NULL) {
            pthread_mutex_unlock(&mutex_io_q);
            io_busy = 0;
            continue;
        }

        PCB *pcb = pop(&IO_Q);

        pthread_mutex_unlock(&mutex_io_q);

        usleep(pcb->IOBurst[pcb->ioIndex] * 1000);  // Assuming IOBurst is in ms
        pcb->ioIndex++;

        pthread_mutex_lock(&mutex_ready_q);
        insert(&Ready_Q, pcb);
        pthread_mutex_unlock(&mutex_ready_q);

        io_busy = 0;
        sem_post(&sem_cpu);
    }

    return NULL;
}



//parse cl arg
ProgramArgs parseArguments(int argc, char *argv[]) {
    ProgramArgs args;
    args.inputFileName = NULL;
    args.algorithm = NULL;
    args.quantum = INT_MIN;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-input") == 0 && i + 1 < argc) {
            args.inputFileName = argv[++i];
        } else if (strcmp(argv[i], "-alg") == 0 && i + 1 < argc) {
            args.algorithm = argv[++i];
            // Check if the algorithm is one of the expected ones
            if (strcmp(args.algorithm, "FIFO") != 0 &&
                strcmp(args.algorithm, "SJF") != 0 &&
                strcmp(args.algorithm, "PR") != 0 &&
                strcmp(args.algorithm, "RR") != 0) {
                fprintf(stderr, "Error: Invalid algorithm specified (%s)\n", args.algorithm);
                exit(1);
            }
        } else if (strcmp(argv[i], "-quantum") == 0 && i + 1 < argc) {
            args.quantum = atoi(argv[++i]);
            if (args.quantum <= 0) {
                fprintf(stderr, "Error: Quantum should be a positive integer.\n");
                exit(1);
            }
        } else {
            fprintf(stderr, "Error: Unknown argument (%s)\n", argv[i]);
            exit(1);
        }
    }

    // Check if mandatory arguments are provided
    if (!args.inputFileName) {
        fprintf(stderr, "Error: Input filename not provided. Use '-input <filename>'.\n");
        exit(1);
    }
    if (!args.algorithm) {
        fprintf(stderr, "Error: Scheduling algorithm not specified. Use '-alg <algorithm>'.\n");
        exit(1);
    }
    if (strcmp(args.algorithm, "RR") == 0 && args.quantum == INT_MIN) {
        fprintf(stderr, "Error: Quantum is required for Round Robin. Use '-quantum <value>'.\n");
        exit(1);
    }

    return args;
}



//main
int main(int argc, char *argv[]) {
    pthread_t fileReadThread, cpuSchedulerThread, ioSystemThread;
    pthread_mutex_init(&mutex_ready_q, NULL);
    pthread_mutex_init(&mutex_io_q, NULL);

    // Initialize semaphores
    sem_init(&sem_cpu, 0, 0);
    sem_init(&sem_io, 0, 0);
    
    // Parse command line arguments
    ProgramArgs args = parseArguments(argc, argv);

    // Validate arguments
    if (!args.inputFileName || !args.algorithm) {
        fprintf(stderr, "Invalid arguments.\n");
        return 1;
    }

    // If RR algorithm is chosen, ensure a valid quantum is provided
    if (strcmp(args.algorithm, "RR") == 0) {
        if (args.quantum == INT_MIN) {
            fprintf(stderr, "For RR algorithm, a valid time quantum must be provided.\n");
            return 1;
        }
    }

    // Create threads
    if(pthread_create(&fileReadThread, NULL, FileRead_thread, (void*) args.inputFileName) != 0) {
        perror("Failed to create fileReadThread");
        return 1;
        }
    if(pthread_create(&cpuSchedulerThread, NULL, CPUscheduler_thread, &args) != 0) {
        perror("Failed to create cpuSchedulerThread");
        return 1;
        }
    if(pthread_create(&ioSystemThread, NULL, IO_system_thread, NULL) != 0) {
        perror("Failed to create ioSystemThread");
        return 1;
        }

    // Wait for threads to finish
    pthread_join(fileReadThread, NULL);
    pthread_join(cpuSchedulerThread, NULL);
    pthread_join(ioSystemThread, NULL);

    // Print results (this is abstracted here)
    printf("...Performance metrics...");

    // Cleanup
    sem_destroy(&sem_cpu);
    sem_destroy(&sem_io);
    pthread_mutex_destroy(&mutex_ready_q);
    pthread_mutex_destroy(&mutex_io_q);

    return 0;
}


