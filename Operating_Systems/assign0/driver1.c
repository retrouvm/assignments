/*
 * File: driver1.c
 * YOU NEED TO IMPLEMENT THE main() + additional functions if needed
 *
 * ....
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

  int choice;
  linked_list_T *list = NewLinkedList();
  
  if (list == NULL) {
    fprintf(stderr, "Error: Failed to initialize the linked list.\n");
    // Handle the error, possibly by exiting the program or taking appropriate action.
}

  do {
      printf("\nMenu:\n");
      printf("1 - Create a new student\n");
      printf("2 - Remove the first student\n");
      printf("3 - Print the number of students\n");
      printf("4 - Print student info by index\n");
      printf("5 - Print the list of all students\n");
      printf("6 - Print min, avg, max GPAs\n");
      printf("7 - Remove student with highest GPA\n");
      printf("8 - Exit\n");
      printf("Enter your choice: ");
      scanf("%d", &choice);

      switch (choice) {
          case 1:
              // Implement adding a new student
              {
                    int id;
                    double gpa;
                    char *name;

                    printf("Enter student ID: ");
                    scanf("%d", &id);

                    // Check if the student ID already exists
                    if (LinkedListContainsID(list, id)) {
                        printf("Student with ID %d already exists.\n", id);
                        break;
                    }

                    printf("Enter student GPA: ");
                    scanf("%lf", &gpa);

                    printf("Enter student name: ");
                    getchar(); // Consume the newline character from the previous input
                    name = ReadLine();

                    // Create a new student cell and add it to the list
                    student_cell_T *element = NewStudentCell(id, gpa, name);
                    Enlist(list, element);
                    printf("Student added successfully.\n");
                }
              break;
          case 2:
              // Implement removing the first student
              {
                    student_cell_T *element = Delist(list);
                    if (element != NULL) {
                        printf("Removed student - ID: %d, GPA: %.2lf, Name: %s\n", element->id, element->gpa, element->name);
                        free(element->name);
                        free(element);
                    } else {
                        printf("The list is empty. Cannot remove a student.\n");
                    }
                }
              break;
          case 3:
              // Implement printing the number of students
              {
                    int numStudents = LinkedListLength(list);
                    printf("Number of students in the list: %d\n", numStudents);
                }
              break;
          case 4:
              // Implement printing student info by index
              {
                    int index;
                    printf("Enter the index of the student: ");
                    scanf("%d", &index);
                    if (list->head == NULL) {
                        fprintf(stderr, "Error: The linked list is empty.\n");
                        // Handle the error, possibly by notifying the user or taking appropriate action.
                    }
                    if (index < 0 || index >= LinkedListLength(list)) {
                        fprintf(stderr, "Error: Invalid index. It is out of bounds.\n");
                        // Handle the error, possibly by notifying the user or taking appropriate action.
                    }
                    student_cell_T *element = GetLinkedListElement(list, index);
                    if (element != NULL) {
                        printf("Student - ID: %d, GPA: %.2lf, Name: %s\n", element->id, element->gpa, element->name);
                    } else {
                        printf("Invalid index. No student found.\n");
                    }
                }
              break;
          case 5:
              // Implement printing the list of all students
              PrintStudentList(list);
              break;
          case 6:
              // Implement printing min, avg, max GPAs
              {
                    double minGPA, avgGPA, maxGPA;
                    CalculateGPAs(list, &minGPA, &avgGPA, &maxGPA);
                    printf("Min GPA: %.2lf\n", minGPA);
                    printf("Average GPA: %.2lf\n", avgGPA);
                    printf("Max GPA: %.2lf\n", maxGPA);
                }
              break;
          case 7:
              // Implement removing student with highest GPA
              {
                    student_cell_T *element = RemoveStudentWithHighestGPA(list);
                    if (element != NULL) {
                        printf("Removed student with highest GPA - ID: %d, GPA: %.2lf, Name: %s\n",
                            element->id, element->gpa, element->name);
                        free(element->name);
                        free(element);
                    } else {
                        printf("The list is empty. Cannot remove a student.\n");
                    }
                }
              break;
          case 8:
              // Implement cleanup and exit
              FreeLinkedList(list); // Free memory
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
  } while (choice != 8);

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

