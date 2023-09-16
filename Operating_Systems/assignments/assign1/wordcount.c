/*
 * File: wordcount.c
 * Retrouvailles Manishimwe ... YOU NEED TO IMPLEMENT THE FUNCTIONS HERE....
 *
 * ....
 */

#include <stdio.h>
#include <stdlib.h>
// include other standard header files or your own user defined libraries needed 
#include <ctype.h>
#include <sys/wait.h>
#include "wordcount.h" // Include the header file

/*
 * YOUR COMMENTS
 */
int countWords(const char *line) {
    int count = 0;
    int inWord = 0;  // 0 if outside a word, 1 if inside a word

    for (int i = 0; line[i] != '\0'; i++) {
        if (isspace(line[i])) {
            inWord = 0;
        } else if (!inWord) {
            inWord = 1;
            count++;
        }
    }
    return count;
}

int main(int argc, char *arvg[])
{
  if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", arvg[0]);
        return 1; // Exit with an error code
    }

    // Open the file for reading
    FILE *file = fopen(arvg[1], "r");
    if (file == NULL) {
        perror("Cannot open file");
        return 1; // Exit with an error code
    }

    char line[2048]; // Assume lines are not longer than 2048 characters
    int totalWords = 0;

    while (fgets(line, sizeof(line), file)) {
        totalWords += countWords(line);
    }

    printf("wordcount with process pid_%d counted words in %s: number of words is %d\n", getpid(), arvg[1], totalWords);

    fclose(file); 
 
  return 0;
}
   