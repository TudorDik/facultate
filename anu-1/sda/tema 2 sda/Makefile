# Compiler setup
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -std=gnu99

# Define target
TARGET = search_index

# Default target: build all
build: $(TARGET)

# Build search_index executable
$(TARGET): system.c utils.h
	$(CC) $(CFLAGS) system.c -o $(TARGET)

# Run target: execute the binary
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET)

.PHONY: build run clean
