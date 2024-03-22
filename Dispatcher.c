#include "Dispatcher.h"

Process* readDispatchList(char* fileName, int *count);

void runDispatcher(Process* jobList, int *count);

void runDispatcher(Process* jobList, int *count) {
	int currentTime = 0;

	// Implement a simple Round Robin scheduler
	int quantum = 1; // Set the quantum to 1 for this example

	while (*count > 0) {
    	for (int i = 0; i < *count; i++) {
        	if (jobList[i].arrivalT <= currentTime && jobList[i].execT > 0) {
            	// Process the job
            	printf("Processing job %d at time %d...\n", i+1, currentTime);
            	jobList[i].execT -= quantum;

            	if (jobList[i].execT <= 0) {
                	printf("Job %d completed.\n", i+1);
                	// Remove the completed job from the list
                	for (int j = i; j < *count - 1; j++) {
                    	jobList[j] = jobList[j + 1];
                	}
                	(*count)--;
                	i--; // Adjust index after removal
            	}
            	currentTime += quantum;
        	}
    	}
	}
}

