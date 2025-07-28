#include "types.h"

void k_print( int iX, int iY, const char* pc_string);

void main(void) {
    k_print(0, 3, "C kernel started.");
    for(;;);
}

void k_print(int x, int y, const char* pc_string) {
    k_char* screen = (void*) 0xB8000;

    screen += (y * 80) + x;
    for(int i=0; pc_string[i] != 0; i++) {
        screen[i].b_char = pc_string[i];
    }
}