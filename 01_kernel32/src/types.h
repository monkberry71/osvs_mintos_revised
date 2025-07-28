#pragma once

#define uint_8 unsigned char
#define uint_16 unsigned short
#define uint_32 unsigned
#define uint_64 unsigned long
#define BOOL unsigned char

#define TRUE 1
#define FALSE 0
#define NULL 0

#pragma pack(push ,1)
typedef struct k_char {
    uint_8 b_char;
    uint_8 b_attr;
} k_char;

#pragma pack(pop)
