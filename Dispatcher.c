#include "Dispatcher.h"
#include <time.h> 
#include <stdarg.h>
// Global queue variables
Process *priorityQueue = NULL;
Process *secondaryQueue = NULL;

ResourceAvailability availableResources = {TOTAL_PRINTERS, TOTAL_SCANNERS, TOTAL_MODEMS, TOTAL_CDS};


#define TOTAL_MEMORY_BLOCKS ((TOTAL_MEMORY - RT_MEMORY_RESERVE) / AVERAGE_PROCESS_SIZE)

MemoryBlock memoryBlocks[TOTAL_MEMORY_BLOCKS];

void initializeMemoryBlocks() {
   
    for (int i = 0; i < TOTAL_MEMORY_BLOCKS; i++) {
        memoryBlocks[i].start = i * AVERAGE_PROCESS_SIZE;
        memoryBlocks[i].size = AVERAGE_PROCESS_SIZE; 
        memoryBlocks[i].isAvailable = true;
    }
}
void setQuantum(Process *process) {
    int quantumSizes[] = {-1, 20, 40, 60}; // Indexed by priority
    process->quantum = (process->priority >= 0 && process->priority <= 3) ? quantumSizes[process->priority] : 0;
}

Process* readDispatchList (char* fileName, int *count){
    FILE *file = fopen(fileName, "r");
    if (!file)
    {
        perror("Failed to open file");
        return NULL;
    }

    char line[256];
    int tempCount = 0;

    // First, count the number of lines
    while (fgets(line, sizeof(line), file)) {
        tempCount++;
    }


    Process *p = (Process *)malloc(tempCount * sizeof(Process));
    if (!p)
    {
        fclose(file);
        return NULL; // Failed to allocate memory.
    }

    fseek(file, 0, SEEK_SET);
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%d, %d, %d, %d, %d, %d, %d, %d",
               &p[i].arrivalT,
               &p[i].priority,
               &p[i].execT,
               &p[i].memMB,
               &p[i].io.print,
               &p[i].io.scan,
               &p[i].io.modem,
               &p[i].io.cd);

       
        setQuantum(&p[i]); // Set the quantum for the process
        i++;
    }

    fclose(file);
    *count = tempCount; // Set the count for the caller.
    return p;
}

// Queue logic here : add, remove, print and sort

void enqueue(Process **head, Process *newProcess) {
    newProcess->next = NULL; // Ensure the new process points to NULL as its next node
    
    if (*head == NULL) {
        // If the queue is empty, the new process becomes the head
        *head = newProcess;
    } else {
        // Find the end of the queue and add the new process there
        Process *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newProcess;
    }
}

Process* dequeue(Process **head) {
    if (*head == NULL) {
        return NULL; // Queue is empty
    }
    
    // Remove the head of the queue
    Process *toReturn = *head;
    *head = toReturn->next;
    
    toReturn->next = NULL; // Isolate the dequeued process
    return toReturn;
}
// resource logic here : block and free resources

bool checkResources(Process *process) {
    // Check if required resources are available
    if (process->io.print <= availableResources.printers &&
        process->io.scan <= availableResources.scanners &&
        process->io.modem <= availableResources.modems &&
        process->io.cd <= availableResources.cds) {
        // Resources are available, deduct them from the available pool
        availableResources.printers -= process->io.print;
        availableResources.scanners -= process->io.scan;
        availableResources.modems -= process->io.modem;
        availableResources.cds -= process->io.cd;
        return true;
    }
    return false; // Resources are not available
}

void freeResources(Process *process) {
    // Free the resources by adding them back to the available pool
    availableResources.printers += process->io.print;
    availableResources.scanners += process->io.scan;
    availableResources.modems += process->io.modem;
    availableResources.cds += process->io.cd;
    logStatus("Freed resources for process", calculateAvailableMemory(), process);
}

bool allocateMemory(Process *process) {
    for (int i = 0; i < TOTAL_MEMORY_BLOCKS; i++) {
        if (memoryBlocks[i].isAvailable && memoryBlocks[i].size >= process->memMB) {
            process->memoryStart = memoryBlocks[i].start;
            memoryBlocks[i].isAvailable = false; // For simplicity, not adjusting the block size
            return true;
        }
    }
    return false;
}

int calculateAvailableMemory() {
    int usedMemory = 0;
    for (int i = 0; i < TOTAL_MEMORY_BLOCKS; i++) {
        if (!memoryBlocks[i].isAvailable) {
            usedMemory += memoryBlocks[i].size;
        }
    }
    return TOTAL_MEMORY - usedMemory;
}

void freeMemory(Process *process) {
    for (int i = 0; i < TOTAL_MEMORY_BLOCKS; i++) {
        if (memoryBlocks[i].start == process->memoryStart) {
            memoryBlocks[i].isAvailable = true;
            break;
        }
    }
    logStatus("Freed memory for process", calculateAvailableMemory(), process);
}

void processQueue(Process **queue) {
    Process *last = NULL, *current = *queue;
    while (current != NULL) {
        if (checkResources(current) && allocateMemory(current)) {
            executeProcess(current);
            freeResources(current);
            freeMemory(current);
            // Remove from queue
            if (last == NULL) *queue = current->next; // Head of the queue changes
            else last->next = current->next;
            
            Process *toDelete = current;
            current = current->next;
            free(toDelete); // Assumes proper deallocation of Process struct
        } else {
            last = current;
            current = current->next;
            // Optionally, re-queue current at the end if resources are unavailable
        }
    }
}

void logStatus(const char* message, int memAvailable, Process* process) {
    // Use time() instead of clock() for real time in seconds
    time_t now = time(NULL);
    printf("[t= %lds ] %s [MEM AVA: %d] — Priority: %d\n",
            now % 60, // To print the seconds part of the minute
            message, 
            memAvailable, 
            process->priority);
}

void executeProcess(Process *process) {
    int memAvailable = calculateAvailableMemory();
    logStatus("Process Starting", memAvailable, process);

    // Simulate process execution
    for (int t = process->execT; t > 0; --t) {
        sleep(1); // Wait for 1 second
        logStatus("Process Running", memAvailable, process);
    }

    logStatus("Process Finished", memAvailable, process);
}
void tryProcessExecution(Process **queue) {
    Process *current = *queue, *prev = NULL;
    while (current) {
        if (checkResources(current) && allocateMemory(current)) {
            executeProcess(current);
            freeResources(current);
            freeMemory(current);
            if (prev) prev->next = current->next;
            else *queue = current->next;
            Process *toDelete = current;
            current = current->next;
            free(toDelete); // Assuming you have a mechanism to properly free Process structs
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void runDispatcher(Process *jobList, int count) {
    // Step 1: Populate the queues
    for (int i = 0; i < count; i++) {
      logStatus("Process Arrived", calculateAvailableMemory(), &jobList[i]);
        if (jobList[i].priority == 0) { // RT process
            enqueue(&priorityQueue, &jobList[i]);
        } else { // User process
            enqueue(&secondaryQueue, &jobList[i]);
        }
    }

    // Step 2: Execute RT processes first
    Process *processToRun;

      tryProcessExecution(&priorityQueue);
printf("inside dispatch 1");
    while ((processToRun = dequeue(&priorityQueue)) != NULL) {
        if (allocateMemory(processToRun)) {
            executeProcess(processToRun);
            freeMemory(processToRun);        }
    }

printf("inside dispatch 2");
    // Step 3: Handle User Processes
      tryProcessExecution(&secondaryQueue);
  
    while ((processToRun = dequeue(&secondaryQueue)) != NULL) {
        if (checkResources(processToRun) && allocateMemory(processToRun)) {
            executeProcess(processToRun);
            freeResources(processToRun); 
      freeMemory(processToRun);
        } else {
        }
    }

printf("inside dispatch 3");
  exit(EXIT_SUCCESS);
}

int main() {
    initializeMemoryBlocks(); // Initialize memory blocks first
    int count = 0;

    // Read jobs from the dispatch list file
    Process* jobList = readDispatchList("processes.txt", &count);
    if (jobList == NULL) {
        fprintf(stderr, "Error reading the dispatch list\n");
        return EXIT_FAILURE;
    }

    // Populate queues with jobs based on their priority
    for (int i = 0; i < count; i++) {
        if (jobList[i].priority == 0) { // RT process
            enqueue(&priorityQueue, &jobList[i]);
        } else { // User process
            enqueue(&secondaryQueue, &jobList[i]);
        }
    }

    // Process the queues
    runDispatcher(jobList, count);

    // Clean up
    free(jobList);

    printf("Dispatcher has finished processing all jobs.\n");
    return 0; // Successful execution
}
