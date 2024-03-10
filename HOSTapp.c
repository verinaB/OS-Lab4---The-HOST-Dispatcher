#include "Dispatcher.h"

int main(void) {
	int count;
	
	process *dispatchL = readDispatchList("dispatch1", &count);
	
	runDispatcher(dispatchL, &count);
}
