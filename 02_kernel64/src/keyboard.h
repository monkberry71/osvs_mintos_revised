#pragma once

#include "types.h"

#define KEY_COUNT_TO_SKIP_FOR_PAUSE 2 
// To handle PAUSE, we need to skip 2 scan codes.

#define KEY_FLAGS_UP 0x00
#define KEY_FLAGS_DOWN 0x01
#define KEY_FLAGS_EXTENDED 0x02

#define KEY_MAPPING_TABLE_MAX_COUNT 89

// ASCII codes

#define KEY_NONE 0x00
#define KEY_ENTER '\n'
#define KEY_TAB '\t'
#define KEY_ESC 0x1B
#define KEY_BACKSPACE 0x08

// This is not ascii, these are our arbitrary codes.
#define KEY_CTRL        0x81
#define KEY_LSHIFT      0x82
#define KEY_RSHIFT      0x83
#define KEY_PRINTSCREEN 0x84
#define KEY_LALT        0x85
#define KEY_CAPSLOCK    0x86
#define KEY_F1          0x87
#define KEY_F2          0x88
#define KEY_F3          0x89
#define KEY_F4          0x8A
#define KEY_F5          0x8B
#define KEY_F6          0x8C
#define KEY_F7          0x8D
#define KEY_F8          0x8E
#define KEY_F9          0x8F
#define KEY_F10         0x90
#define KEY_NUMLOCK     0x91
#define KEY_SCROLLLOCK  0x92
#define KEY_HOME        0x93
#define KEY_UP          0x94
#define KEY_PAGEUP      0x95
#define KEY_LEFT        0x96
#define KEY_CENTER      0x97
#define KEY_RIGHT       0x98
#define KEY_END         0x99
#define KEY_DOWN        0x9A
#define KEY_PAGEDOWN    0x9B
#define KEY_INS         0x9C
#define KEY_DEL         0x9D
#define KEY_F11         0x9E
#define KEY_F12         0x9F
#define KEY_PAUSE       0xA0

#pragma pack(push,1)
typedef struct k_key_mapping_entry {
    uint8_t normal_code; // 1,2,3,4
    uint8_t combined_code; // if shift or caps lock, ! @ # $
} k_key_mapping_entry;
#pragma pack(pop)

typedef struct k_keyboard_manager {
    BOOL shift_down;
    BOOL caps_lock_on;
    BOOL num_lock_on;
    BOOL scroll_lock_on;

    BOOL is_processing_extended_code;
    int skip_count_for_pause;
} k_keyboard_manager;

/**
 * @brief  Check if the keyboard controller's output buffer is full.
 * @return TRUE if there is data to read from port 0x60, else FALSE.
 */
BOOL k_is_output_buffer_full(void);

/**
 * @brief  Check if the keyboard controller's input buffer is full.
 * @return TRUE if the controller is busy (port 0x60 not ready for a new write), else FALSE.
 */
BOOL k_is_input_buffer_full(void);

/**
 * @brief  Activate the PS/2 keyboard device.
 * @return TRUE on successful activation (received ACK), else FALSE.
 */
BOOL k_activate_keyboard(void);

/**
 * @brief  Read one raw scan‐code byte from the keyboard.
 * @return The scan‐code read from port 0x60.
 */
uint8_t k_get_scan_code(void);

/**
 * @brief  Determine whether to use the “shifted”/combined code for a given scan‐code.
 * @param  scan_code  The raw scan‐code byte (make or break).
 * @return TRUE if the combined-code (shift/CAPS/NUM lock) should be used, else FALSE.
 */
BOOL k_is_using_combined_code(uint8_t scan_code);

/**
 * @brief  Update internal shift/CAPS/NUM lock state and (optionally) LED status.
 * @param  scan_code  The raw scan‐code byte (make or break).
 */
void update_combination_key_status_and_led(uint8_t scan_code);

/**
 * @brief  Convert a raw scan‐code into an ASCII (or special) code and flags.
 * @param  scan_code   The raw scan‐code byte read from the keyboard.
 * @param  out_ascii   Pointer to receive the resulting ASCII or special KEY_* code.
 * @param  out_flags   Pointer to receive flags (KEY_FLAGS_DOWN, KEY_FLAGS_EXTENDED).
 * @return TRUE if a valid ASCII/KEY code was produced, FALSE if this byte should be ignored.
 */
BOOL k_convert_SC_to_ASCII(uint8_t scan_code,
                           uint8_t *out_ascii,
                           BOOL    *out_flags);