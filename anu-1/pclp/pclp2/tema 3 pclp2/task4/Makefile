.PHONY: clean

CC = gcc
ASM = nasm

CFLAGS = -Wall -g -Wpointer-arith
ASMFLAGS = -f elf64 -g
LDFLAGS = -no-pie

TARGET_EXEC = langford_checker

C_SRCS = $(wildcard *.c)
ASM_SRCS = $(wildcard *.asm)

C_OBJS = $(C_SRCS:.c=.o)
ASM_OBJS = $(ASM_SRCS:.asm=.o)

OBJS = $(C_OBJS) $(ASM_OBJS)

$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	$(RM) *.o *.d $(TARGET_EXEC)
