#include "util.h"
#include "types.h"
#include <stdarg.h>

int k_strlen(const char* str) {
    int i;
    for(i=0;;i++) 
        if(str[i] == '\0') break;
    return i;
}

static uint64_t gs_total_RAM_in_MB = 0;

void k_check_total_RAM(void) {
    uint32_t* curr_addr;
    uint32_t prev_val;
    curr_addr = (void*) 0x4000000;
    for(;;) {
        prev_val = *curr_addr;
        *curr_addr = 0x12345678;
        if(*curr_addr != 0x12345678) break;
        *curr_addr = prev_val;
        curr_addr += (0x400000 / 4);
    }
    gs_total_RAM_in_MB = (uint64_t) curr_addr / 0x100000;
}

uint64_t k_get_total_RAM_size(void) {
    return gs_total_RAM_in_MB;
}

long k_atoi(const char* buffer, int radix) {
    long to_return;
    switch(radix) {

        case 16:
            to_return = k_HEX_str_to_long(buffer);
            break;
        default:
        case 10:
            to_return = k_DEC_str_to_long(buffer);
            break;
    }
    return to_return;
}

long k_HEX_str_to_long(const char* buffer) {
    long value = 0;
    for(int i=0; buffer[i] != '\0'; i++) {
        value *= 16;

        if('A' <= buffer[i] && buffer[i] <= 'Z') value += (buffer[i] - 'A') + 10;
        else if ('a' <= buffer[i] && buffer[i] <= 'z') value += (buffer[i] - 'a') + 10;
        else value += buffer[i] - '0';
    }
    return value;
}

long k_DEC_str_to_long(const char* buffer) {
    long value = 0;
    int i = buffer[0] == '-' ? 1 : 0;

    for(; buffer[i] != '\0'; i++) {
        value *= 10;
        value += buffer[i] - '0';
    }

    value = buffer[0] == '-' ? -value : value;
    return value;
}

int k_itoa(long value, char* buffer, int radix) {
    int to_return;
    switch(radix) {
        case 16:
            to_return = k_HEX_to_str(value, buffer);
            break;
        case 10:
            to_return = k_DEC_to_str(value, buffer);
            break;
    }

    return to_return;
}

int k_HEX_to_str(uint64_t value, char* buffer) {
    if(!value) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return 1;
    }

    int i;
    for(i=0; value > 0; i++) {
        uint64_t curr_val = value % 16;
        if(curr_val >= 10) {
            buffer[i] = 'A' + (curr_val - 10);
        } else {
            buffer[i] = '0' + curr_val;
        }

        value /= 16;
    }

    buffer[i] = '\0';
    k_reverse_str(buffer);
    return i;
}

int k_DEC_to_str(long value, char* buffer) {
    long i;

    if(!value) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return 1;
    }

    if(value < 0) {
        i = 1;
        buffer[0] = '-';
        value = -value;
    } else {
        i = 0;
    }

    for(; value > 0; i++) {
        buffer[i] = '0' + value % 10;
        value = value / 10;
    }
    buffer[i] = '\0';
    k_reverse_str(buffer[0] == '-' ? &(buffer[1]):buffer);
    return i;
}

void k_reverse_str(char* buffer) {
    int length = k_strlen(buffer);

    for(int i=0; i< length / 2; i++) {
        char temp = buffer[i];
        buffer[i] = buffer[length - 1 - i];
        buffer[length - 1 - i] = temp;
    }
}

int k_sprintf(char* buffer, const char* format_str,...) {
    va_list ap;
    int i_return;

    va_start(ap, format_str);
    i_return = k_vsprintf(buffer, format_str, ap);
    va_end(ap);

    return i_return;
}

int k_vsprintf(char* buffer, const char* format_str, va_list ap) {
    int format_length = k_strlen(format_str);
    int buffer_index = 0;
    for(int i=0; i < format_length; i++) {
        if(format_str[i] == '%') {
            switch(format_str[++i]) {
                case 's': {
                    char* copy_str = (void*) (va_arg(ap, char*));
                    int copy_length = k_strlen(copy_str);
    
                    k_memcpy(buffer + buffer_index, copy_str, copy_length);
                    buffer_index += copy_length;
                    break;
                    }

                case 'c':
                    buffer[buffer_index] = (char) (va_arg(ap, int));
                    buffer_index++;
                    break;
                
                case 'd':
                case 'i': {
                    int value = (int) (va_arg(ap, int));
                    buffer_index += k_itoa(value, buffer + buffer_index, 10);
                    break;
                }
                
                case 'x':
                case 'X': {

                    uint64_t value = (uint32_t) (va_arg(ap, uint32_t)) & 0XFFFFFFFF;
                    buffer_index += k_itoa(value, buffer + buffer_index, 16);
                    break;
                }

                case 'q':
                case 'Q':
                case 'p': {
                    uint64_t value = (uint64_t) (va_arg(ap, uint64_t));
                    buffer_index += k_itoa(value, buffer + buffer_index, 16);
                    break;
                }

                default:
                    buffer[buffer_index] = format_str[i];
                    buffer_index++;
                    break;
            }
        } else {
            buffer[buffer_index] = format_str[i];
            buffer_index++;
        }
    }

    buffer[buffer_index] = '\0';
    return buffer_index;
}

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
        if (temp) return (int) temp;
    }
    return 0; 
}
