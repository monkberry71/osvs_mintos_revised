#include <stdarg.h>
#include "console.h"
#include "keyboard.h"
#include "util.h"
#include "k_inout.h"

k_console_manager gs_st_console_manager = {0,};

void k_set_cursor(int x, int y) {
    // set cursor to specified pos
    int linear_val = y * CONSOLE_WIDTH + x;

    k_outb(VGA_PORT_INDEX, VGA_INDEX_UPPER_CURSOR); // send 0x0E(VGA_INDEX_UPPER_CURSOR) to CRTC contorll addr register(VGA_PORT_INDEX), selecting upper cursor
    k_outb(VGA_PORT_DATA, linear_val >> 8);

    k_outb(VGA_PORT_INDEX, VGA_INDEX_LOWER_CURSOR);
    k_outb(VGA_PORT_DATA, linear_val & 0xFF);

    gs_st_console_manager.curr_print_offset = linear_val;
}

void k_init_console(int x, int y) {
    k_memset(&gs_st_console_manager, 0, sizeof(gs_st_console_manager));
    k_set_cursor(x,y);
}

void k_get_cursor(int *x, int *y) {
    *x = gs_st_console_manager.curr_print_offset % CONSOLE_WIDTH;
    *y = gs_st_console_manager.curr_print_offset / CONSOLE_WIDTH;
}

int k_console_print_string(const char* buffer) {
    k_char* screen = (void*) VGA_STARTING_ADDR;

    int print_offset = gs_st_console_manager.curr_print_offset;
    int length = k_strlen(buffer);

    for(int i=0; i < length; i++) {
        if (buffer[i] == '\n') {
            print_offset += (CONSOLE_WIDTH - (print_offset % CONSOLE_WIDTH));
        } else if (buffer[i] == '\t') {
            print_offset += (8 - (print_offset % CONSOLE_WIDTH));
        } else {
            screen[print_offset].b_attr = CONSOLE_DEFAULT;
            screen[print_offset].b_char = buffer[i];
            print_offset++;
        }
        
        // scroll if it is at last line
        if ( print_offset >= (CONSOLE_HEIGHT * CONSOLE_WIDTH) ) {
            // copy [1:] lines to first line.
            k_memcpy(VGA_STARTING_ADDR, VGA_STARTING_ADDR + sizeof(k_char) * CONSOLE_WIDTH, (CONSOLE_HEIGHT - 1) * sizeof(k_char) * CONSOLE_WIDTH );

            // delete last line
            for(int j=0;j<CONSOLE_WIDTH;j++) {
                screen[(CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH + j].b_char = ' ';
                screen[(CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH + j].b_attr = CONSOLE_DEFAULT;
            }

            print_offset = (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH;
        }
    }
    
    return print_offset;
}

void k_printf(const char* format_string, ...) {
    va_list ap;
    char vc_buffer[1024];
    int next_print_offset;

    va_start(ap, format_string);
    k_vsprintf(vc_buffer, format_string, ap);
    va_end(ap);

    next_print_offset = k_console_print_string(vc_buffer);

    k_set_cursor(next_print_offset % CONSOLE_WIDTH, next_print_offset / CONSOLE_WIDTH);
    
}

void k_clear_screen(void) {
    k_char* screen = (void*) VGA_STARTING_ADDR;
    for(int i=0; i< CONSOLE_HEIGHT * CONSOLE_WIDTH; i++) {
        screen[i].b_char = ' ';
        screen[i].b_attr = CONSOLE_DEFAULT;
    }
    k_set_cursor(0,0);
}

uint8_t k_getch(void) {
    k_key_data data;

    for(;;) {
        while(k_get_key_from_key_queue(&data) == FALSE);

        if(data.flags & KEY_FLAGS_DOWN) return data.ascii;
    }
}

void k_print_string_xy(int x, int y, const char* str) {
    k_char* screen = (void*) VGA_STARTING_ADDR;
    screen += 80 * y + x;
    for(int i=0; str[i] != 0; i++) {
        screen[i].b_char = str[i];
        screen[i].b_attr = CONSOLE_DEFAULT;
    }
}