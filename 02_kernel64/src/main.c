#include "types.h"

void k_print(int x, int y, const char* str);

void main(void) {
    k_print(0, 10, "IA-32e Mode C kernel Starting.");
}

void k_print(int x, int y, const char* str) {
    k_char* screen = (void*) 0xB8000;
    screen += (80 * y) + x;

    for(int i=0; str[i] != 0; i++) screen[i].b_char = str[i];
}