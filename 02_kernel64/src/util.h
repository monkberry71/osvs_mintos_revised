#pragma once

#include "types.h"

void k_memset(void* , uint8_t, int);
int k_memcpy(void*, const void*, int);
int k_memcmp(const void*, const void*, int);
void k_print(int x, int y, const char* str);
void k_print_num(int x, int y, unsigned char num);