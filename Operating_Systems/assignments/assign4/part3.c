#include <stdio.h>
#include <stdlib.h>
#include <math.h> // Needed for calculating the log base 2

typedef struct {
    int valid_bit;
    int frame_number;
    int last_used_time; // For LRU tracking
} PageTableEntry;

// Global variables replaced by dynamic parameters
int OFFSET_SIZE;
int PAGE_TABLE_ENTRIES;
int FRAME_COUNT;
int *revmap;

// Function to find a free frame or replace one using LRU policy
int find_free_frame(PageTableEntry *PT, int *freeframes, int *revmap, int *LRUcount, int *current_time) {
    for (int i = 1; i < FRAME_COUNT; i++) {
        if (freeframes[i]) {
            //printf("Allocating free frame: %d for page: %d\n", i, revmap[i]);
            return i;
        }
    }

    int lru_index = 1;
    for (int i = 2; i < FRAME_COUNT; i++) {
        if (LRUcount[i] < LRUcount[lru_index]) {
            lru_index = i;
        }
    }
    // Print statement goes here, before you invalidate the old page table entry
    //printf("Replacing page: %d (frame %d last used at time %d) with new page\n",
           //revmap[lru_index], lru_index, LRUcount[lru_index]);

    PT[revmap[lru_index]].valid_bit = 0;
    revmap[lru_index] = -1;
    LRUcount[lru_index] = (*current_time)++;
    return lru_index;
}

unsigned long translate(PageTableEntry *PT, unsigned long LA, int *freeframes, int *revmap, int *LRUcount, int *current_time, int *page_faults) {
    unsigned long pnum = LA >> OFFSET_SIZE;
    unsigned long dnum = LA & ((1 << OFFSET_SIZE) - 1);
    if (PT[pnum].valid_bit == 0) {
        //printf("Page fault for page number: %lu\n", pnum);
        (*page_faults)++;
        int frame = find_free_frame(PT, freeframes, revmap, LRUcount, current_time);
        PT[pnum].valid_bit = 1;
        PT[pnum].frame_number = frame;
        freeframes[frame] = 0;
        revmap[frame] = pnum;
        LRUcount[frame] = (*current_time)++;
        return ((unsigned long)frame << OFFSET_SIZE) | dnum;
    }
    else{
        LRUcount[PT[pnum].frame_number] = (*current_time)++;
        return ((unsigned long)PT[pnum].frame_number << OFFSET_SIZE) | dnum;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s BytesPerPage SizeOfVirtualMemory SizeOfPhysicalMemory infile outfile\n", argv[0]);
        return EXIT_FAILURE;
    }

    int BytesPerPage = atoi(argv[1]);
    int SizeOfVirtualMemory = atoi(argv[2]);
    int SizeOfPhysicalMemory = atoi(argv[3]);
    char *infile_name = argv[4];
    char *outfile_name = argv[5];

    OFFSET_SIZE = (int)log2(BytesPerPage);
    PAGE_TABLE_ENTRIES = SizeOfVirtualMemory / BytesPerPage;
    FRAME_COUNT = SizeOfPhysicalMemory / BytesPerPage;

    // Dynamic allocation of PageTable and other structures
    PageTableEntry *PT = calloc(PAGE_TABLE_ENTRIES, sizeof(PageTableEntry));
    int *freeframes = calloc(FRAME_COUNT, sizeof(int));
    int *LRUcount = calloc(FRAME_COUNT, sizeof(int));
    int current_time = 0, page_faults = 0;
    // Dynamic allocation of revmap
    revmap = calloc(FRAME_COUNT, sizeof(int));
    if (!revmap) {
        perror("Failed to allocate memory for revmap");
        // Free other allocated resources if any
        free(PT);
        free(freeframes);
        free(LRUcount);
        return EXIT_FAILURE;
    }

    // Initialize revmap
    for (int i = 0; i < FRAME_COUNT; i++) {
        revmap[i] = -1;
    }

    // Initialize free frames (assuming frame 0 is reserved for the OS)
    freeframes[0] = 0; // OS reserved
    for (int i = 1; i < FRAME_COUNT; ++i) {
        freeframes[i] = 1; // Free frame
    }

    FILE *infile = fopen(infile_name, "rb");
    if (!infile) {
        perror("Error opening infile");
        return EXIT_FAILURE;
    }
    FILE *outfile = fopen(outfile_name, "wb"); // Corrected to use outfile_name
    if (!outfile) {
        perror("Error opening outfile");
        fclose(infile);
        return EXIT_FAILURE;
    }

    unsigned long LA, PA;
    while (fread(&LA, sizeof(unsigned long), 1, infile) == 1) {
        PA = translate(PT, LA, freeframes, revmap, LRUcount, &current_time, &page_faults);
        fwrite(&PA, sizeof(unsigned long), 1, outfile);
    }

    fclose(infile);
    fclose(outfile);

    // Free dynamically allocated memory
    free(PT);
    free(freeframes);
    free(revmap); // Don't forget to free revmap
    free(LRUcount);

    // Print out the number of page faults
    printf("Page faults: %d\n", page_faults);

    return EXIT_SUCCESS;
}
