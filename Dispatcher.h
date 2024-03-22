#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int printers;
    int scanners;
    int modems;
    int cds;
} ResourceAvailability;

#define TOTAL_PRINTERS 2
#define TOTAL_SCANNERS 1
#define TOTAL_MODEMS 1
#define TOTAL_CDS 2

extern ResourceAvailability availableResources;

typedef struct IOdev {
    int print;
    int scan;
    int modem;
    int cd;
} IOdev;

typedef struct MemoryBlock {
    int start;
    int size;
    bool isAvailable;
} MemoryBlock;

// Assuming a definition for TOTAL_MEMORY_BLOCKS, which should match your dispatcher.c setup
#define TOTAL_MEMORY 1024 // Total memory in MBytes
#define RT_MEMORY_RESERVE 64 // Reserved memory for RT processes
#define AVERAGE_PROCESS_SIZE 64 // Assuming each process takes up 64 MB on average, adjust as needed
//#define TOTAL_MEMORY_BLOCKS (TOTAL_MEMORY - RT_MEMORY_RESERVE) / AVERAGE_PROCESS_SIZE // Example calculation

//extern MemoryBlock memoryBlocks[TOTAL_MEMORY_BLOCKS]; // Assuming external definition

typedef struct Process {
    int arrivalT;
    int priority;
    int execT;
    int memMB;
    IOdev io;
    int quantum;
    struct Process* next; // For queue linking
    // Add memory start if not included in your dispatcher.c
    int memoryStart; // Memory block start index for the process
} Process;

// Function declarations
Process* readDispatchList(char* fileName, int *count);
void runDispatcher(Process* jobList, int count);
void enqueue(Process **head, Process *newProcess);
Process* dequeue(Process **head);
void setQuantum(Process *process);
void initializeMemoryBlocks(void); // If your setup requires initialization of memory blocks
bool allocateMemory(Process *process);
void freeMemory(Process *process);
void freeResources(Process *process);
void executeProcess(Process *process);
void tryProcessExecution(Process **queue); // For managing and executing processes in the queue

void logStatus(const char* message, int memAvailable, Process* process);
int calculateAvailableMemory(void);
void initializeMemoryBlocks(void);
#endif // DISPATCHER_H

