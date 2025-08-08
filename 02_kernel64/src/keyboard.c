#include "types.h"
#include "k_inout.h"
#include "keyboard.h"
#include "queue.h"
#include "interrupt_stack.h"
#include "interrupt_helper.h"

#define KEYCON_CONTROL_REGISTER 0x64
#define KEYCON_STATUS_REGISTER 0x64
#define KEYCON_INPUT_BUFFER 0x60
#define KEYCON_OUTPUT_BUFFER 0x60

#define IS_SCAN_CODE_ALPHABET(s) ('a' <= gs_SC_to_ASCII_table[(s)].normal_code && gs_SC_to_ASCII_table[(s)].normal_code <= 'z')
#define IS_SCAN_CODE_NUM_OR_SYMBOL(s) ( ( 2 <= (s) ) && ( (s) <= 53 ) && !IS_SCAN_CODE_ALPHABET(s) )
#define IS_SCAN_CODE_NUMPAD(s) ( 71 <= (s) && (s) <= 83 )


BOOL k_is_output_buffer_full(void) {
    return (k_inb(KEYCON_STATUS_REGISTER) & 0x01) ? TRUE : FALSE; // if 0x01 is set, it is full.
}

BOOL k_is_input_buffer_full(void) {
    return (k_inb(KEYCON_STATUS_REGISTER) & 0x02) ? TRUE : FALSE; // if 0x02 is set, it is full.
}

BOOL k_activate_keyboard(void) {
    push_cli();
    k_outb(KEYCON_CONTROL_REGISTER, 0xAE); // say to controller register that you activate keyboard.
    for(int i=0; i<0xFFFF; i++) {
        // maybe there could be already something in the input buffer, so we firstly wait until it is empty.
        if(!k_is_input_buffer_full()) break;
    }
    // we waited long enough
    k_outb(KEYCON_INPUT_BUFFER, 0xF4); // activate keyboard itself.

    for(int j=0; j<100; j++) {

        // wait until the output buffer is full
        for(int i=0; i<0xFFFF; i++) {
            if(k_is_output_buffer_full()) break;
        }

        // 0xFA is ACK.
        if (k_inb(KEYCON_OUTPUT_BUFFER) == 0xFA) {
            pop_cli(); 
            return TRUE;
        }
    }
    pop_cli();
    return FALSE; // activation failed.
}

uint8_t k_get_scan_code(void) {
    while(!k_is_output_buffer_full()); // wait until it is full.

    return k_inb(KEYCON_OUTPUT_BUFFER);
}

static k_keyboard_manager gs_keyboard_manager = {0,};

static Queue gs_key_queue;
static k_key_data gs_key_queue_array[KEY_MAX_QUEUE_COUNT];

static k_key_mapping_entry gs_SC_to_ASCII_table[KEY_MAPPING_TABLE_MAX_COUNT] = {
    [ 0] = { KEY_NONE,       KEY_NONE       },
    [ 1] = { KEY_ESC,        KEY_ESC        },

    /* Number row */
    [ 2] = { '1',            '!'            },
    [ 3] = { '2',            '@'            },
    [ 4] = { '3',            '#'            },
    [ 5] = { '4',            '$'            },
    [ 6] = { '5',            '%'            },
    [ 7] = { '6',            '^'            },
    [ 8] = { '7',            '&'            },
    [ 9] = { '8',            '*'            },
    [10] = { '9',            '('            },
    [11] = { '0',            ')'            },
    [12] = { '-',            '_'            },
    [13] = { '=',            '+'            },

    /* Editing keys */
    [14] = { KEY_BACKSPACE,  KEY_BACKSPACE  },
    [15] = { KEY_TAB,        KEY_TAB        },

    /* Q – P row */
    [16] = { 'q',            'Q'            },
    [17] = { 'w',            'W'            },
    [18] = { 'e',            'E'            },
    [19] = { 'r',            'R'            },
    [20] = { 't',            'T'            },
    [21] = { 'y',            'Y'            },
    [22] = { 'u',            'U'            },
    [23] = { 'i',            'I'            },
    [24] = { 'o',            'O'            },
    [25] = { 'p',            'P'            },
    [26] = { '[',            '{'            },
    [27] = { ']',            '}'            },
    [28] = { '\n',           '\n'           },  /* Enter */

    /* Control & Caps */
    [29] = { KEY_CTRL,       KEY_CTRL       },
    [30] = { 'a',            'A'            },
    [31] = { 's',            'S'            },
    [32] = { 'd',            'D'            },
    [33] = { 'f',            'F'            },
    [34] = { 'g',            'G'            },
    [35] = { 'h',            'H'            },
    [36] = { 'j',            'J'            },
    [37] = { 'k',            'K'            },
    [38] = { 'l',            'L'            },
    [39] = { ';',            ':'            },
    [40] = { '\'',           '\"'           },
    [41] = { '`',            '~'            },

    [42] = { KEY_LSHIFT,     KEY_LSHIFT     },
    [43] = { '\\',           '|'            },

    /* Z – M row */
    [44] = { 'z',            'Z'            },
    [45] = { 'x',            'X'            },
    [46] = { 'c',            'C'            },
    [47] = { 'v',            'V'            },
    [48] = { 'b',            'B'            },
    [49] = { 'n',            'N'            },
    [50] = { 'm',            'M'            },
    [51] = { ',',            '<'            },
    [52] = { '.',            '>'            },
    [53] = { '/',            '?'            },

    [54] = { KEY_RSHIFT,     KEY_RSHIFT     },
    [55] = { '*',            '*'            },  /* NumPad * */

    [56] = { KEY_LALT,       KEY_LALT       },
    [57] = { ' ',            ' '            },  /* Space */
    [58] = { KEY_CAPSLOCK,   KEY_CAPSLOCK   },

    /* Function keys */
    [59] = { KEY_F1,         KEY_F1         },
    [60] = { KEY_F2,         KEY_F2         },
    [61] = { KEY_F3,         KEY_F3         },
    [62] = { KEY_F4,         KEY_F4         },
    [63] = { KEY_F5,         KEY_F5         },
    [64] = { KEY_F6,         KEY_F6         },
    [65] = { KEY_F7,         KEY_F7         },
    [66] = { KEY_F8,         KEY_F8         },
    [67] = { KEY_F9,         KEY_F9         },
    [68] = { KEY_F10,        KEY_F10        },

    /* NumLock / ScrollLock */
    [69] = { KEY_NUMLOCK,    KEY_NUMLOCK    },
    [70] = { KEY_SCROLLLOCK, KEY_SCROLLLOCK },

    /* Numpad & navigation */
    [71] = { KEY_HOME,       '7'            },
    [72] = { KEY_UP,         '8'            },
    [73] = { KEY_PAGEUP,     '9'            },
    [74] = { '-',            '-'            },  /* NumPad - */
    [75] = { KEY_LEFT,       '4'            },
    [76] = { KEY_CENTER,     '5'            },
    [77] = { KEY_RIGHT,      '6'            },
    [78] = { '+',            '+'            },  /* NumPad + */
    [79] = { KEY_END,        '1'            },
    [80] = { KEY_DOWN,       '2'            },
    [81] = { KEY_PAGEDOWN,   '3'            },
    [82] = { KEY_INS,        '0'            },
    [83] = { KEY_DEL,        '.'            },

    /* Unused/reserved */
    [84] = { KEY_NONE,       KEY_NONE       },
    [85] = { KEY_NONE,       KEY_NONE       },
    [86] = { KEY_NONE,       KEY_NONE       },

    [87] = { KEY_F11,        KEY_F11        },
    [88] = { KEY_F12,        KEY_F12        },
};

BOOL k_init_keyboard(void) {
    k_init_queue(&gs_key_queue, gs_key_queue_array, KEY_MAX_QUEUE_COUNT, sizeof(k_key_data));
    return k_activate_keyboard();
}

BOOL k_convert_SC_and_put_queue(uint8_t sc) {
    k_key_data key_data;
    BOOL result = FALSE;
    
    key_data.scancode = sc;

    if ( k_convert_SC_to_ASCII( sc, &(key_data.ascii), &(key_data.flags) ) ) {
        push_cli();
        result = k_put_queue(&gs_key_queue, &key_data);
        pop_cli();
    }

    return result;
}

BOOL k_get_key_from_key_queue(k_key_data* data) {
    // k_int_3();
    if (k_is_queue_empty(&gs_key_queue)) return FALSE;
    push_cli();
    BOOL result =k_get_queue(&gs_key_queue, data);
    pop_cli();
    return result;
}

BOOL k_is_using_combined_code(uint8_t scan_code) {
    // find whether scan_code is affected by other keys.
    uint8_t down_scan_code  = scan_code & 0x7F; // first bit is 1 == going up, first bit is 0 == goind down.
    BOOL should_use_combined_key = FALSE;

    // if it is alphabet
    if(IS_SCAN_CODE_ALPHABET(down_scan_code)) {
        // if Shift or caps lock key is pressed, return combined key
        should_use_combined_key = gs_keyboard_manager.shift_down ^ gs_keyboard_manager.caps_lock_on ? TRUE: FALSE;
    } else if (IS_SCAN_CODE_NUM_OR_SYMBOL(down_scan_code)) {
        should_use_combined_key = gs_keyboard_manager.shift_down ? TRUE : FALSE;
    } else if (IS_SCAN_CODE_NUMPAD(down_scan_code)) {
        should_use_combined_key = gs_keyboard_manager.num_lock_on ? TRUE : FALSE;
    }

    return should_use_combined_key;
}

void update_combination_key_status_and_led(uint8_t scan_code) {
    BOOL key_down = !(scan_code & 0x80);
    uint8_t down_scan_code = scan_code & 0x7F;
    BOOL should_led_status_change = FALSE;
    // combined key search
    // shift 
    if ( down_scan_code == 42 || down_scan_code == 54 ) gs_keyboard_manager.shift_down = key_down; // 42 is l shift, 54 is r shift, and it follows whether the key is pressed or not, so it effectively follows shift key pressed or not.
    else if ( down_scan_code == 58 && key_down) {
        // caps lock
        gs_keyboard_manager.caps_lock_on ^= TRUE;
        should_led_status_change = TRUE;
    } else if ( down_scan_code == 69 && key_down) {
        gs_keyboard_manager.num_lock_on ^= TRUE;
        should_led_status_change = TRUE;
    } else if ( down_scan_code == 70 && key_down) {
        gs_keyboard_manager.num_lock_on ^= TRUE;
        should_led_status_change = TRUE;
    }

    if(should_led_status_change) {
        // not gonna do it.
    }
}

BOOL k_convert_SC_to_ASCII(uint8_t scan_code, uint8_t* out_ascii, BOOL* out_flags) {
    BOOL should_use_combined_key;

    if(gs_keyboard_manager.skip_count_for_pause > 0) {
        // PAUSE was introduced, so we will ignore 1 or 2 byte.
        gs_keyboard_manager.skip_count_for_pause--;
        return FALSE;
    }

    // PAUSE is introduced justly.
    if (scan_code == 0xE1) {
        *out_ascii = KEY_PAUSE;
        *out_flags = KEY_FLAGS_DOWN;
        gs_keyboard_manager.skip_count_for_pause = KEY_COUNT_TO_SKIP_FOR_PAUSE;
        return TRUE;
    }

    // extended key
    else if (scan_code == 0xE0) {
        gs_keyboard_manager.is_processing_extended_code = TRUE;
        return FALSE;
    }

    should_use_combined_key = k_is_using_combined_code(scan_code);

    *out_ascii = should_use_combined_key ? gs_SC_to_ASCII_table[scan_code & 0x7F].combined_code : gs_SC_to_ASCII_table[scan_code & 0x7F].normal_code;

    if (gs_keyboard_manager.is_processing_extended_code) {
        *out_flags = KEY_FLAGS_EXTENDED;
        gs_keyboard_manager.is_processing_extended_code = FALSE;
    } else {
        *out_flags = 0;
    }

    if ( !(scan_code & 0x80) ) *out_flags |= KEY_FLAGS_DOWN; // if it the key is downed, set down flags,

    update_combination_key_status_and_led(scan_code);
    return TRUE;
}

