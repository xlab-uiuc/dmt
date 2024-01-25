#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char* mem_chunk = NULL;
    unsigned long size_g = 2;
    unsigned long size_B = 0;

    if (*argv == NULL) {
        return -1;
    }

    printf("DMT/Hyperlane simple memory allocator\n");

    if ((argc != 1 && argc != 2) || (argc == 2 && !sscanf(argv[1], "%lu", &size_g))) {
        printf("Usage: %s [size(GB)]\n", argv[0]);
        return -1;
    }
    size_B = size_g * 1024 * 1024 * 1024;

    printf("Allocating %lu GB (%lu bytes)\n", size_g, size_B);
    mem_chunk = malloc(size_B);
    if (!mem_chunk) {
        printf("Unable to allocate memory\n");
        return -1;
    }

    printf("Setting memory\n");
    memset(mem_chunk, 0, size_B);

    printf("Memory ready to review\nPress ENTER key to free and exit...\n");  
    getchar();  

    printf("Freeing memory\n");
    free(mem_chunk);

    printf("Goodbye\n");
    return 0;
}
