# Copyright PCLP Team, 2025

# Compiler setup
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 

# Define targets
TARGETS = battleships

# Default target: build all
build: $(TARGETS)

# Run target: execute battleships
run: battleships
	./battleships

# Build battleships executable
battleships: battleships.c
	$(CC) $(CFLAGS) -o $@ $<

# Pack the solution into a zip file
pack:
	zip -FSr 313CD_DicuTudor_Tema2.zip README Makefile *.c *.h

# Clean build artifacts
clean:
	rm -f $(TARGETS)

.PHONY: build run pack clean