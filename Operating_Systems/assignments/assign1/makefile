CC = gcc
CFLAGS = -Wall -g

all: wordcount multiple_wordcount 

clean:
	rm -f *.o wordcount multiple_wordcount

wordcount: wordcount.c wordcount.h
	$(CC) $(CFLAGS) $^ -o $@

multiple_wordcount: multiple_wordcount.c wordcount.h
	$(CC) $(CFLAGS) $^ -o $@

