# Makefile for the scheduling program

# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -pthread

# Target Executable
TARGET = scheduling

all: $(TARGET)

$(TARGET): scheduling.c
	$(CC) $(CFLAGS) scheduling.c -o $(TARGET)

clean:
	rm -f $(TARGET)
