#include "types.h"
#include "keyboard.h"

void k_print(int x, int y, const char* str);
void k_print_num(int x, int y, unsigned char num);

void main(void) {
    char vc_temp[2] = {0,0};
    uint8_t flags = 0;
    int i=0;

    k_print(0, 11, "IA-32e Mode C kernel Starting.");

    k_print(0, 12, "Keyboard activation...");
    if (k_activate_keyboard()) {
        k_print(0, 12, "Keyboard activation...good");
    } else {
        k_print(0, 12, "Keyboard is fked up.");
        for(;;);
    }

    for(;;) {
        if(k_is_output_buffer_full()) {
            uint8_t temp_SC = k_get_scan_code();

            if (k_convert_SC_to_ASCII(temp_SC, &( vc_temp[0]), &flags)) {
                if((flags & KEY_FLAGS_DOWN)) {
                    if ((unsigned) vc_temp[0] < (unsigned) 0x80) k_print(i++, 13, vc_temp);
                    k_print_num(50, 0, vc_temp[0]);
                }
            }
    }
}
}

void k_print(int x, int y, const char* str) {
    k_char* screen = (void*) 0xB8000;
    screen += (80 * y) + x;

    for(int i=0; str[i] != 0; i++) screen[i].b_char = str[i];
}

void k_print_num(int x, int y, unsigned char num) {
    k_char* screen = (void*) 0xB8000;

    screen += (y*80) + x;
    // screen[0].b_char = num;
    screen[0].b_char = (char) (num % 10 + '0');
    screen[-1].b_char = (char) ((num / 10) % 10 + '0');
    screen[-2].b_char = (char) ((num / 100) % 10 + '0');
    screen[-3].b_char = (char) ((num / 1000) % 10 + '0');
}