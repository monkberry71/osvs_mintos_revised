#include "shell.h"
#include "console.h"
#include "keyboard.h"
#include "util.h"

k_shell_cmd_entry gs_cmd_table[] = {
    {"help", "Show help", k_shell_help},
    {"clear", "Clear screen", k_shell_clear},
    {"totalram", "Show total RAM size", k_shell_totalram},
    {"strtod", "string to decimal/hex converter", k_shell_string_to_decimal_hex_test},
    {"shutdown", "shutdown and reboot OS", k_shutdown}
};

void k_exec_cmd(const char* cmd_buffer) {
    int cmd_buffer_length = k_strlen(cmd_buffer);
    int first_space_index;
    for(first_space_index=0; first_space_index < cmd_buffer_length; first_space_index++) {
        if(cmd_buffer[first_space_index] == ' ') break;
    }

    int count = sizeof(gs_cmd_table) / sizeof(k_shell_cmd_entry);

    int i=0;
    for(i=0; i < count; i++) {
        int cmd_length = k_strlen(gs_cmd_table[i].cmd);
        if(cmd_length == first_space_index && k_memcmp(gs_cmd_table[i].cmd, cmd_buffer, first_space_index) == 0) {
            (*gs_cmd_table[i].function)(cmd_buffer + first_space_index + 1);
            break;
        } 
    }
    if (i >= count) 
        k_printf("%s is not found.\n", cmd_buffer);
}

void k_start_shell (void) {
    char cmd_buffer[SHELL_MAX_CMD_BUFFER_COUNT];
    int cmd_buffer_index = 0;
    k_printf(SHELL_PROMPT_MSG);
    int cursor_x, cursor_y;

    for(;;) {
        uint8_t key = k_getch();
        if (key == KEY_BACKSPACE) {
            if(cmd_buffer_index > 0) {
                k_get_cursor(&cursor_x, &cursor_y);
                k_print_string_xy(cursor_x -1, cursor_y, " ");
                k_set_cursor(cursor_x - 1, cursor_y);
                cmd_buffer_index--;
            }
        } else if (key == KEY_ENTER) {
            k_printf("\n");
            if (cmd_buffer_index > 0) {
                cmd_buffer[cmd_buffer_index] = '\0';
                k_exec_cmd(cmd_buffer);
            }
        } else if (key == KEY_LSHIFT || key == KEY_RSHIFT || key == KEY_CAPSLOCK || key == KEY_NUMLOCK || key == KEY_SCROLLLOCK);
        else {
            if (key == KEY_TAB) key = ' ';
            if (cmd_buffer_index < SHELL_MAX_CMD_BUFFER_COUNT) {
                cmd_buffer[cmd_buffer_index++] = key;
                k_printf("%c", key);
            }
        }
    }
}

void k_init_param(k_param_list* list, const char* in_param_str) {
    list->buffer = in_param_str;
    list->length = k_strlen(in_param_str);
    list->curr_pos = 0;
}

int k_get_next_param(k_param_list* list, char* param) {
    // param will have the argument string after this function.
    if(list->length <= list->curr_pos) return 0;

    int i;
    for(i = list->curr_pos; i < list->length; i++) {
        if(list->buffer[i] == ' ') break;
    }

    k_memcpy(param, list->buffer + list->curr_pos, i);
    int length = i - list->curr_pos;
    param[length] = '\0';

    list->curr_pos += length + 1;
    return length;
}

void k_shell_help(const char* cmd_buffer) {
    k_printf("=================\n");
    k_printf("Let me help you!!\n");
    k_printf("=================\n");

    int count = sizeof(gs_cmd_table) / sizeof(k_shell_cmd_entry);

    int length, max_cmd_length = 0;
    for(int i=0;i < count; i++) {
        length = k_strlen(gs_cmd_table[i].cmd);
        if (length > max_cmd_length) max_cmd_length = length;
    }


    int cursor_x, cursor_y;

    for(int i=0;i < count; i++) {
        k_printf("%s", gs_cmd_table[i].cmd);
        k_get_cursor(&cursor_x, &cursor_y);
        k_set_cursor(max_cmd_length, cursor_y);
        k_printf(" - %s\n", gs_cmd_table[i].help);
    }
}

void k_shell_clear(const char* param_buffer) {
    k_clear_screen();
    k_set_cursor(0,1);
}

void k_shell_totalram(const char* param_buffer) {
    k_printf("Total RAM size = %dMB\n", k_get_total_RAM_size());
}

void k_shell_string_to_decimal_hex_test(const char* param_buffer) {
    char param[100];
    k_param_list param_list;
    int count = 0;
    long target_val;
    
    k_init_param(&param_list, param_buffer);


    for(;;) {
        int length = k_get_next_param(&param_list, param);
        if (length == 0) break;

        k_printf("Param %d = %s, length = %d, ", count + 1, param, length);

        if(k_memcmp(param, "0x", 2) == 0) {
            target_val = k_atoi(param+2, 16);
            k_printf("HEX value = %q\n", target_val);
        } else {
            target_val = k_atoi(param, 10);
            k_printf("Decimal Value = %d\n", target_val);
        }

        count++;
    }
}

void k_shutdown(const char* param_buffer) {
    k_printf("System Shutdown start...Press any key to go.\n");

    k_getch();
    // k_reboot();
}