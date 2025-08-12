#pragma once

#include "types.h"

#define SHELL_MAX_CMD_BUFFER_COUNT 300
#define SHELL_PROMPT_MSG "$ "

typedef void (*cmd_func) (const char* pc_param);

#pragma pack(push,1)
typedef struct k_shell_cmd_entry {
    char* cmd;
    char* help;
    cmd_func function;
} k_shell_cmd_entry;

typedef struct k_param_list {
    const char* buffer;
    int length;
    int curr_pos;
} k_param_list;

// Main shell control
void k_exec_cmd(const char* cmd_buffer);
void k_start_shell(void);

// Parameter parsing
void k_init_param(k_param_list* list, const char* in_param_str);
int  k_get_next_param(k_param_list* list, char* param);

// Built-in shell commands
void k_shell_help(const char* cmd_buffer);
void k_shell_clear(const char* param_buffer);
void k_shell_totalram(const char* param_buffer);
void k_shell_string_to_decimal_hex_test(const char* param_buffer);
void k_shutdown(const char* param_buffer);