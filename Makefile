# Executable name
EXEC_NAME=dispatcher

# Source and Header files
SRCS=dispatcher.c
HEADERS=dispatcher.h

# Compiler
CC=gcc
CFLAGS=-Wall -g

# For debugging
DEBUGGER=clang
DEBUGFLAGS=-fsanitize=address -O1 -fno-omit-frame-pointer

# Default build
$(EXEC_NAME): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o $(EXEC_NAME)

# Debug build
debug: $(SRCS) $(HEADERS)
	$(DEBUGGER) $(DEBUGFLAGS) $(CFLAGS) $(SRCS) -o $(EXEC_NAME)DEBUG

# Clean
clean:
	rm -f *.o $(EXEC_NAME) $(EXEC_NAME)DEBUG

