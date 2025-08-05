#include "util.h"
#include "types.h"

void k_memset(void *dest, uint8_t data, int size) {
    for(int i=0; i<size; i++) {
        ( (uint8_t*) dest )[i] = data;
    }
}

int k_memcpy(void* dest, const void* src, int size) {
    for(int i=0; i<size; i++) {
        ( (uint8_t*) dest )[i] = ( (uint8_t *) src)[i];
    }

    return size;
}

int k_memcmp(const void* dest, const void* src, int size) {
    for(int i=0; i<size; i++) {
        char temp = ((char*) dest)[i] - ((char*) src)[i];
        if (!temp) return (int) temp;
    }
    return 0; 
}
