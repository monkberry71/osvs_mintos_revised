#pragma once

#include "types.h"

// Foreground colors (text color)
#define VGA_FG_BLACK         0x00
#define VGA_FG_BLUE          0x01
#define VGA_FG_GREEN         0x02
#define VGA_FG_CYAN          0x03
#define VGA_FG_RED           0x04
#define VGA_FG_MAGENTA       0x05
#define VGA_FG_BROWN         0x06
#define VGA_FG_LIGHT_GREY    0x07
#define VGA_FG_DARK_GREY     0x08
#define VGA_FG_LIGHT_BLUE    0x09
#define VGA_FG_LIGHT_GREEN   0x0A
#define VGA_FG_LIGHT_CYAN    0x0B
#define VGA_FG_LIGHT_RED     0x0C
#define VGA_FG_LIGHT_MAGENTA 0x0D
#define VGA_FG_YELLOW        0x0E
#define VGA_FG_WHITE         0x0F

// Background colors (shifted left by 4 bits)
#define VGA_BG_BLACK         0x00
#define VGA_BG_BLUE          0x10
#define VGA_BG_GREEN         0x20
#define VGA_BG_CYAN          0x30
#define VGA_BG_RED           0x40
#define VGA_BG_MAGENTA       0x50
#define VGA_BG_BROWN         0x60
#define VGA_BG_LIGHT_GREY    0x70
#define VGA_BG_DARK_GREY     0x80
#define VGA_BG_LIGHT_BLUE    0x90
#define VGA_BG_LIGHT_GREEN   0xA0
#define VGA_BG_LIGHT_CYAN    0xB0
#define VGA_BG_LIGHT_RED     0xC0
#define VGA_BG_LIGHT_MAGENTA 0xD0
#define VGA_BG_YELLOW        0xE0
#define VGA_BG_WHITE         0xF0

// Blink attribute
#define VGA_BLINK            0x80

#define CONSOLE_DEFAULT ( VGA_BG_BLACK | VGA_FG_WHITE )

#define CONSOLE_WIDTH 80
#define CONSOLE_HEIGHT 25
#define VGA_STARTING_ADDR 0xB8000

#define VGA_PORT_INDEX 0x3D4
#define VGA_PORT_DATA 0x3D5
#define VGA_INDEX_UPPER_CURSOR 0x0E
#define VGA_INDEX_LOWER_CURSOR 0x0F

#pragma pack(push, 1)

typedef struct k_console_manager {
    int curr_print_offset;
} k_console_manager;

#pragma pack(pop)

// Set cursor to specified position
void k_set_cursor(int x, int y);

// Initialize console with cursor position
void k_init_console(int x, int y);

// Get current cursor position
void k_get_cursor(int *x, int *y);

// Print string to console (handles \n and \t), returns next print offset
int k_console_print_string(const char* buffer);

// printf-style formatted console output
void k_printf(const char* format_string, ...);

// Clear entire screen and reset cursor
void k_clear_screen(void);

// Wait for a key press and return its ASCII value
uint8_t k_getch(void);

// Print a string at a specific x,y position
void k_print_string_xy(int x, int y, const char* str);

