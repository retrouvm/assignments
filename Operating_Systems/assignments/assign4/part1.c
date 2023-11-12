#include <stdio.h>
#include <stdlib.h>

// Define the size of the page/frame
#define OFFSET_SIZE 7

// Declare the static page table as per the assignment description
int PT[8] = {2, 4, 1, 7, 3, 5, 6, -1};

// Function to translate logical to physical address
unsigned long translate(unsigned long LA) {
    // Extract the page number and the offset from the logical address
    unsigned long pnum = LA >> OFFSET_SIZE;
    unsigned long dnum = LA & 0x7F;

    // Get the frame number from the page table
    int fnum = PT[pnum];

    // If the frame number is -1, it means that the page is not present in the physical memory
    if (fnum == -1) {
        fprintf(stderr, "Page fault!\n");
        exit(EXIT_FAILURE);
    }

    // Construct the physical address
    unsigned long PA = ((unsigned long)fnum << OFFSET_SIZE) | dnum;
    return PA;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the input file in binary read mode
    FILE *infile = fopen(argv[1], "rb");
    if (!infile) {
        perror("Error opening infile");
        return EXIT_FAILURE;
    }

    // Open the output file in binary write mode
    FILE *outfile = fopen(argv[2], "wb");
    if (!outfile) {
        perror("Error opening outfile");
        fclose(infile);
        return EXIT_FAILURE;
    }

    unsigned long LA, PA;

    // Read each logical address from the input file, translate it and write the physical address to the output file
    while (fread(&LA, sizeof(unsigned long), 1, infile) == 1) {
        PA = translate(LA);
        fwrite(&PA, sizeof(unsigned long), 1, outfile);
        // For testing purposes, uncomment the line below to see the LA and PA on the screen
        printf("LA = %lx   PA = %lx \n", LA, PA);
    }

    // Close the input and output files
    fclose(infile);
    fclose(outfile);

    return EXIT_SUCCESS;
}
