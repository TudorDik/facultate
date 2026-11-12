# Compiler setup
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -std=gnu99

# Define targets
TARGETS = tema1

# Default target: build all
build: $(TARGETS)

# Run target: execute tema1
run: $(TARGETS)
	./$(TARGETS)

# Build tema1 executable
$(TARGETS): tema1.c list.h
	$(CC) $(CFLAGS) tema1.c -o $(TARGETS)

# Clean build artifacts
clean:
	rm -f $(TARGETS)

.PHONY: build run pack clean