/*
 * File: driver1.c
 * YOU NEED TO IMPLEMENT THE main() + additional functions if needed
 *
 * ....
 */

#include <stdio.h>
#include <stdlib.h>
#include "mylinkedlist.h"

/* 
 * Function: ReadLine
 * Usage: s = ReadLine();
 * ---------------------
 * ReadLine reads a line of text from standard input and returns
 * the line as a string.  The newline '\n' character that terminates
 * the input is not stored as part of the string.
 */
char *ReadLine(void);



 
int main(int argc, char *arvg[])
{
  // YOU NEED TO IMPLEMENT THIS driver1.c USING FUNCTIONS FROM mylinkedlist.h
  // But before that, implement your ReadLine() function, and test it as shown below. 
  // IF NEEDED, YOU CAN ALSO IMLEMENT YOUR OWN FUNCTIONS HERE

  char *name;

  printf("Enter a name to test your ReadLine function  : ");
  name = ReadLine();
  if (name != NULL){
    printf("User entered : %s \n", name);  
    free(name);
  }
  else {
    printf("Memory allocation error\n");
    }

  return 0;
}

/* 
 * IMPLEMENTATION of ReadLine();
 * Function: ReadLine
 * Usage: s = ReadLine();
 * ---------------------
 * ReadLine reads a line of text from standard input and returns
 * the line as a string.  The newline '\n' character that terminates
 * the input is not stored as part of the string.
 *
 * In contrast to standard I/O functions (e.g., scanf with "%s", fgets) 
 * that can read strings into a given static size buffer, ReadLine function 
 * should read the given input line of characters terminated by a newline 
 * character ('\n') into a dynamically allocated and resized buffer based on 
 * the length of the given input line. 
 *
 * When implementing this function you can use standard I/O functions. 
 * We just want you to make sure you allocate enough space for the entered data. 
 * So don't simply allocate 100 or 1000 bytes every time. 
 * If the given input has 5 characters, you need to allocate space for 6 characters.
 *
 * Hint: initially dynamically allocate an array of char with size 10. 
 * Then, read data into that array character by charecter (e.g, you can use getchar()). 
 * If you see '\n' char before reading 10th character, you are done. And you know the 
 * exact length of the input string. So, accordingly allocate enough space and copy the 
 * data into new char array, insert '\0' instead of '\n' and free the original array. 
 * Then return the new string. However, if you DO NOT see '\n' char after 10th character, 
 * then you need larger space. Accordingly, resize your original array and double its size 
 * and continue reading data character by character as in the first step... 
 * Hope you got the idea! 
 *
 * Also please check for possible errors (e.g., no memory etc.) and appropriately handle 
 * them. For example, if malloc returns NULL, free partially allocated space and return 
 * NULL from this function. The program calling this function may take other actions, 
 * e.g., stop the program!
 */
char *ReadLine()
{
  int initialBufferSize = 10;
    char *buffer = (char *)malloc(initialBufferSize); // Allocate initial buffer
    if (buffer == NULL) {
        return NULL; // Return NULL on memory allocation failure
    }

    int size = initialBufferSize;
    int length = 0;
    char c;

    while (1) {
        c = getchar(); // Read one character at a time

        if (c == '\n' || c == EOF) {
            break; // Stop if newline or end of file is encountered
        }

        buffer[length] = c;
        length++;

        if (length == size - 1) {
            // If buffer is full, resize it by doubling its size
            size *= 2;
            char *newBuffer = (char *)realloc(buffer, size);
            if (newBuffer == NULL) {
                free(buffer);
                return NULL; // Return NULL on memory allocation failure
            }
            buffer = newBuffer;
        }
    }

    buffer[length] = '\0'; // Null-terminate the string

    // Resize the buffer to fit the actual length of the string
    char *finalBuffer = (char *)realloc(buffer, length + 1);
    if (finalBuffer == NULL) {
        free(buffer);
        return NULL; // Return NULL on memory allocation failure
    }

    return finalBuffer;
}

   