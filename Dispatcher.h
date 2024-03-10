#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <stdarg.h>

typdef struct {
	int print;
	int scan;
	int modem;
	int cd;
} IOdev;

typedef struct {
	int arrivalT;
	int priority; //0 RT, 1-3 User
	int execT; 
	int memMB;
	IOdev io; 
} Process;

Process* readDispatchList(char* fileName, int *count);

void runDispatcher(Process* jobList, int *count); 
