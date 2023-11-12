#include <stdio.h>
#include <stdlib.h>

#define OFFSET_SIZE 7 // Define the size of the offset based on page/frame size
#define PAGE_TABLE_ENTRIES 32 // Number of pages
#define FRAME_COUNT 8 // Number of frames

typedef struct {
    int valid_bit;
    int frame_number;
} PageTableEntry;

PageTableEntry PT[PAGE_TABLE_ENTRIES];
int freeframes[FRAME_COUNT] = {0, 1, 1, 1, 1, 1, 1, 1};
int revmap[FRAME_COUNT] = {-1, -1, -1, -1, -1, -1, -1, -1};
int LRUcount[FRAME_COUNT] = {0};
int current_time = 0;
int page_faults = 0;

int find_free_frame() {
    for (int i = 1; i < FRAME_COUNT; i++) {
        if (freeframes[i]) return i;
    }

    int lru_index = 1;
    for (int i = 2; i < FRAME_COUNT; i++) {
        if (LRUcount[i] < LRUcount[lru_index]) {
            lru_index = i;
        }
    }

    PT[revmap[lru_index]].valid_bit = 0;
    revmap[lru_index] = -1;
    LRUcount[lru_index] = current_time;
    return lru_index;
}

unsigned long translate(unsigned long LA) {
    unsigned long pnum = LA >> OFFSET_SIZE;
    unsigned long dnum = LA & ((1 << OFFSET_SIZE) - 1);
    if (PT[pnum].valid_bit == 1) {
        LRUcount[PT[pnum].frame_number] = current_time++;
        return ((unsigned long)PT[pnum].frame_number << OFFSET_SIZE) | dnum;
    } else {
        page_faults++;
        int frame = find_free_frame();
        PT[pnum].valid_bit = 1;
        PT[pnum].frame_number = frame;
        freeframes[frame] = 0;
        revmap[frame] = pnum;
        LRUcount[frame] = current_time++;
        return ((unsigned long)frame << OFFSET_SIZE) | dnum;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *infile = fopen(argv[1], "rb");
    if (!infile) {
        perror("Error opening infile");
        return EXIT_FAILURE;
    }

    FILE *outfile = fopen(argv[2], "wb");
    if (!outfile) {
        perror("Error opening outfile");
        fclose(infile);
        return EXIT_FAILURE;
    }

    unsigned long LA, PA;
    while (fread(&LA, sizeof(unsigned long), 1, infile) == 1) {
        PA = translate(LA);
        fwrite(&PA, sizeof(unsigned long), 1, outfile);
    }

    fclose(infile);
    fclose(outfile);

    // Print out the number of page faults
    printf("Page faults: %d\n", page_faults);

    return EXIT_SUCCESS;
}
