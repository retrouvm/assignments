CC = gcc
CFLAGS = -Wall -Werror -std=c99 -lm

all: part1 part2 part3

part1: part1.c
	$(CC) $(CFLAGS) part1.c -o part1

part2: part2.c
	$(CC) $(CFLAGS) part2.c -o part2

part3: part3.c
	$(CC) $(CFLAGS) part3.c -o part3 -lm

clean:
	rm -f part1 part2 part3
