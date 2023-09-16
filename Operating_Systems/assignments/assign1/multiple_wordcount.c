/*
 * File: multiple_wordcount.c
 * YOUR NAME ... YOU NEED TO IMPLEMENT THE FUNCTIONS HERE....
 *
 * ....
 */

#include <stdio.h>
#include <stdlib.h>
// include other standard header files or your own user defined libraries needed 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "wordcount.h" // Include the header file

/*
 * YOUR COMMENTS
 */


int main(int argc, char *arvg[])
{
  if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename1> <filename2> ... <filen>\n", arvg[0]);
        return 1;
    }

    int numFiles = argc - 1;
    int successCount = 0;
    int notExistCount = 0;

    printf("Parent process pid created %d child processes to count words in %d files\n", numFiles, numFiles);

    for (int i = 1; i <= numFiles; i++) {
        pid_t cpid = fork();

        if (cpid == -1) {
            perror("fork");
            exit(1);
        }

        if (cpid == 0) {
            // Child process
            execl("./wordcount", "./wordcount", arvg[i], NULL);
            // If execl fails, the child continues executing this code
            fprintf(stderr, "exec failed for file %s\n", arvg[i]);
            exit(2);
        }
    }

    // Parent process
    for (int i = 0; i < numFiles; i++) {
        int status;
        wait(&status);

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0) {
                successCount++;
            } else if (WEXITSTATUS(status) == 1) {
                notExistCount++;
            }
        }
    }

    printf("%d files have been counted successfully!\n", successCount);
    printf("%d files did not exist\n", notExistCount); 
 
  return 0;
}
   