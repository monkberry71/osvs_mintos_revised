#pragma once

#include "types.h"
#include <stdarg.h>

int k_strlen(const char* str);

void k_check_total_RAM(void);
uint64_t k_get_total_RAM_size(void);

long k_atoi(const char* buffer, int radix);
long k_HEX_str_to_long(const char* buffer);
long k_DEC_str_to_long(const char* buffer);

int k_itoa(long value, char* buffer, int radix);
int k_HEX_to_str(uint64_t value, char* buffer);
int k_DEC_to_str(long value, char* buffer);

void k_reverse_str(char* buffer);

int k_sprintf(char* buffer, const char* format_str, ...);
int k_vsprintf(char* buffer, const char* format_str, va_list ap);

void k_memset(void* dest, uint8_t data, int size);
int k_memcpy(void* dest, const void* src, int size);
int k_memcmp(const void* dest, const void* src, int size);