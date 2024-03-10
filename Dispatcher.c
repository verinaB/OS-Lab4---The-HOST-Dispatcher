#include "Dispatcher.h"

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
               &p[i].io.modems,
               &p[i].io.cd);

        // Set the correct quantum for the process based on its priority level

        i++;
    }

    fclose(file);
    *count = tempCount; // Set the count for the caller.
    return p;
}

// Queue logic here : add, remove, print and sort

// resource logic here : block and free resources

void runDispatcher (Process* jobList, int *count){

}

