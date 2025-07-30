#include "types.h"
#include "page.h"
#include "mode_switch.h"

void k_print( int iX, int iY, const char* pc_string);
BOOL k_init_kernel64_area(void);
BOOL k_is_memory_enough(void);

void main(void) {

    uint32_t dwEAX, dwEBX, dwECX, dwEDX;
    char vc_vendor_str[13] = {0};

    k_print(0, 3, "hi, C kernel started.");
    k_print(0, 4, "Minimum mem size checking...");
    if ( !k_is_memory_enough() ) {
        k_print(0, 4, "Minium mem size checking...failed.");
        for(;;);
    } else k_print(0, 4, "Minimum mem size checking...good.");


    if (k_init_kernel64_area()) {
        k_print(0, 5, "IA-32e kernel area initialization completed.");
    } else {
        k_print(0, 5, "64 init failed.");
    }

    k_print(0,6, "IA-32e page tables initializing...");
    k_initialize_page_tables();
    k_print(0,6, "IA_32e_page_tables_initializing...Done!");

    k_read_CPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
    *((uint32_t*) vc_vendor_str) = dwEBX;
    *((uint32_t*) vc_vendor_str + 1) = dwEDX;
    *((uint32_t*) vc_vendor_str + 2) = dwECX;
    k_print(0, 7, "Processor Vendor Str check...");
    k_print(45, 7, vc_vendor_str);

    k_read_CPUID(0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX);
    k_print(0, 8, "64bit support check...");
    if (dwEDX & (1 << 29)) k_print(0,8,"64bit support check...ok!");
    else {
        k_print(0,9, "CPU doesn't support 64bit mode.");
        for(;;);
    }
    k_print(0,9, "Going 64bit");
    for(;;);
}

void k_print(int x, int y, const char* pc_string) {
    k_char* screen = (void*) 0xB8000;

    screen += (y * 80) + x;
    for(int i=0; pc_string[i] != 0; i++) {
        screen[i].b_char = pc_string[i];
    }
}

BOOL k_init_kernel64_area(void) {
    uint32_t* curr_addr;

    curr_addr = (void*) 0x100000; // 0x100000 is where the kernel64 is to be placed.

    while( (uint32_t) curr_addr < 0x600000) {
        *curr_addr = 0x00;

        if (*curr_addr != 0) return FALSE;
        curr_addr++;
    }
    return TRUE;
}

BOOL k_is_memory_enough(void) {
    uint32_t* curr_addr = (void*) 0x100000;
    while( (uint32_t)curr_addr < 64 * 0x100000 ) {
        *curr_addr = 0x12345678;
        if (*curr_addr != 0x12345678) return FALSE;
        curr_addr += (0x100000 / 4);
    }
    return TRUE;
}