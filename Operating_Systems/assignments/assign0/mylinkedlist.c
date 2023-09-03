/*
 * File: mylinkedlist.c 
 * YOU NEED TO IMPLEMENT THE FUNCTIONS  here
 * ....
 */

#include <stdio.h>
#include <stdlib.h>
#include "mylinkedlist.h"
 
/*
 * Function: NewStudentCell
 * Usage: student_cell_T *element;
 *        element = NewStudentCell(int id, double gpa, char *name);
 * --------------------------
 * This function allocates space for a student cell and intilize its fileds
 */
student_cell_T *NewStudentCell(int id, double gpa, char *name)
{
   student_cell_T *element;
  
   element = (student_cell_T *) malloc( sizeof(student_cell_T) );
   if( !element){
     fprintf(stderr,"NewStudentCell cannot allocate memory\n");
     return NULL;
   }
   element->id = id;
   element->gpa = gpa;
   element->name = name;

  return element;
}


/*
 * Function: NewLinkedList
 * Usage: linked_list_T *list;
 *        list = NewLinkedList();
 * --------------------------
 * This function allocates and returns an empty linked list.
 */
linked_list_T *NewLinkedList(void)
{
   linked_list_T *list;
  
   list = (linked_list_T *) malloc( sizeof(linked_list_T) );
   if( !list){
     fprintf(stderr,"NewLinkedList cannot allocate memory\n");
     return NULL;
   }

   list->head = NULL;
   list->tail = NULL;

  return list;
}


/*
 * Function: FreeLinkedList
 * Usage: FreeLinkedList(list);
 * ------------------------
 * This function frees the storage associated with list.
 */
void FreeLinkedList(linked_list_T *list)
{
  student_cell_T *current = list->head;
    while (current != NULL) {
        student_cell_T *next = current->next; // Store the next cell before freeing the current one
        free(current->name); // Free the name (char*) field
        free(current); // Free the student cell
        current = next; // Move to the next cell
    }
    free(list); // Finally, free the linked list structure itself
}

/*
 * Function: Enlist
 * Usage: Enlist(list, element);
 * -------------------------------
 * This function adds a student cell pointed by element to the end of the list.
 */
void Enlist(linked_list_T *list, student_cell_T *element)
{
  // Ensure that the element is not NULL
    if (element == NULL) {
        fprintf(stderr, "Error: Cannot add a NULL element to the list.\n");
        return;
    }

    // Initialize the next pointer of the element to NULL
    element->next = NULL;

    if (list->head == NULL) {
        // If the list is empty, set both head and tail to the new element
        list->head = element;
        list->tail = element;
    } else {
        // If the list is not empty, append the element to the end
        list->tail->next = element;
        list->tail = element;
    }
}

/*
 * Function: Delist
 * Usage: element = Delist(list);
 * --------------------------------
 * This function removes the student cell at the head of the list
 * and returns its address to the caller (client).  If the list is empty, Delist
 * prints an Error with an appropriate message and returns NULL.
 */
student_cell_T *Delist(linked_list_T *list)
{
  // Check if the list is empty
    if (list->head == NULL) {
        fprintf(stderr, "Error: The list is empty. Cannot remove a student.\n");
        return NULL;
    }

    // Get the head cell to be removed
    student_cell_T *element = list->head;

    // Update the head pointer to point to the next cell in the list
    list->head = element->next;

    // If the list becomes empty after removal, update the tail pointer to NULL
    if (list->head == NULL) {
        list->tail = NULL;
    }

    // Return the removed element
    return element;
}

/*
 * Functions: LinkedListIsEmpty, LinkedListIsFull
 * Usage: if (LinkedListIsEmpty(list)) . . .
 *        if (LinkedListIsFull(list)) . . .
 * -------------------------------------
 * These functions test whether the list is empty or full.
 */
int LinkedListIsEmpty(linked_list_T *list)
{
  // Check if the head pointer is NULL, indicating an empty list
  return list->head == NULL;
}

int LinkedListIsFull(linked_list_T *list)
{
  return 0; // because we have linked list
}

/*
 * Function: LinkedListLength
 * Usage: n = LinkedListLength(list);
 * ------------------------------
 * This function returns the number of elements in the list.
 */
int LinkedListLength(linked_list_T  *list)
{
  int count = 0;
    student_cell_T *current = list->head;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}

/*
 * Function: GetLinkedListElement
 * Usage: element = GetLinkedListElement(list, index);
 * -----------------------------------------------
 * This function returns the element at the specified index in the
 * list, where the head of the list is defined as index 0. For
 * example, calling GetLinkedListElement(list, 0) returns the initial
 * element from the list without removing it.  If the caller tries
 * to select an element that is out of range, GetLinkedListElement prints
 * Error and returns NULL.  Note: This function is not a fundamental list operation
 * and is instead provided mainly to facilitate debugging.
 */
student_cell_T *GetLinkedListElement(linked_list_T *list, int index)
{
  if (index < 0) {
        printf("Error: Index cannot be negative.\n");
        return NULL;
    }

    student_cell_T *current = list->head;
    int currentIndex = 0;

    while (current != NULL) {
        if (currentIndex == index) {
            return current; // Return the student cell at the specified index
        }
        current = current->next;
        currentIndex++;
    }

    // If the loop finishes without finding the element, it means the index is out of range
    printf("Error: Index out of range.\n");
    return NULL;
}



/* OTHER FUNCTIONS YOU WOULD NEED....
 * EXPORT THEM HERE, BUT IMPLMENT THEM in mylinkedlist.c 
 */

int LinkedListContainsID(linked_list_T *list, int id)
{
  student_cell_T *current = list->head;

    while (current != NULL) {
        if (current->id == id) {
            return 1; // ID exists in the linked list
        }
        current = current->next;
    }

    // ID not found in the linked list
    return 0;
}


void PrintStudentList(linked_list_T *list)
{
  student_cell_T *current = list->head;

    if (current == NULL) {
        printf("The student list is empty.\n");
        return;
    }

    printf("List of Students:\n");
    while (current != NULL) {
        printf("ID: %d, GPA: %.2lf, Name: %s\n", current->id, current->gpa, current->name);
        current = current->next;
    }
}


void CalculateGPAs(linked_list_T *list, double *minGPA, double *avgGPA, double *maxGPA)
{
  student_cell_T *current = list->head;

    if (current == NULL) {
        // If the list is empty, set all GPA values to 0.0
        *minGPA = 0.0;
        *avgGPA = 0.0;
        *maxGPA = 0.0;
        return;
    }

    // Initialize variables to calculate min, avg, and max GPAs
    double totalGPA = 0.0;
    int numStudents = 0;
    *minGPA = current->gpa;
    *maxGPA = current->gpa;

    while (current != NULL) {
        // Update total GPA for calculating average
        totalGPA += current->gpa;

        // Update min and max GPAs
        if (current->gpa < *minGPA) {
            *minGPA = current->gpa;
        }
        if (current->gpa > *maxGPA) {
            *maxGPA = current->gpa;
        }

        // Move to the next student cell
        current = current->next;
        numStudents++;
    }

    // Calculate the average GPA
    *avgGPA = totalGPA / numStudents;
}


student_cell_T *RemoveStudentWithHighestGPA(linked_list_T *list)
{
  if (list->head == NULL) {
        // The list is empty, cannot remove a student
        return NULL;
    }

    // Initialize variables to keep track of the student with the highest GPA
    student_cell_T *current = list->head;
    student_cell_T *studentWithHighestGPA = current;
    double highestGPA = current->gpa;

    while (current->next != NULL) {
        current = current->next;
        if (current->gpa > highestGPA) {
            highestGPA = current->gpa;
            studentWithHighestGPA = current;
        }
    }

    // Remove the student with the highest GPA from the list
    if (studentWithHighestGPA == list->head) {
        list->head = list->head->next;
    } else {
        current = list->head;
        while (current->next != studentWithHighestGPA) {
            current = current->next;
        }
        current->next = studentWithHighestGPA->next;
    }

    studentWithHighestGPA->next = NULL; // Disconnect the removed student cell
    return studentWithHighestGPA;
}
