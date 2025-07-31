#pragma once

#define uint8_t unsigned char
#define uint16_t unsigned short
#define uint32_t unsigned
#define uint64_t unsigned long
#define BOOL unsigned char

#define TRUE 1
#define FALSE 0
#define NULL 0

#pragma pack(push ,1)
typedef struct k_char {
    uint8_t b_char;
    uint8_t b_attr;
} k_char;

#pragma pack(pop)
