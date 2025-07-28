#pragma once
#include "types.h"

#define PAGE_FLAGS_P   0x00000001 // Present
#define PAGE_FLAGS_RW  0x00000002 // R/W
#define PAGE_FLAGS_US  0x00000004 // User/Supervisor
#define PAGE_FLAGS_PWT 0x00000008 // Page level write-through
#define PAGE_FLAGS_PCD 0x00000010 // Page level cache-disable
#define PAGE_FLAGS_A   0x00000020 // Accessed
#define PAGE_FLAGS_D   0x00000040 // Dirty
#define PAGE_FLAGS_PS  0x00000080 // Page size 2MB vs 4KB
#define PAGE_FLAGS_G   0x00000100 // Global
#define PAGE_FLAGS_PAT 0x00001000 // page attribute table index
#define PAGE_FLAGS_EXB 0x80000000 // execution bit
#define PAGE_FLAGS_DEFAULT ( PAGE_FLAGS_P | PAGE_FLAGS_RW)
#define PAGE_TABLE_SIZE 0x1000 // 512 * 8 = 2^12 
#define PAGE_MAX_ENTRY_COUNT 512
#define PAGE_DEFAULT_SIZE 0x200000 // 2MB 

#pragma pack(push, 1)
typedef struct k_page_table_entry_struct {
    uint32_t attr_and_lower_base_addr;
    uint32_t upper_base_addr_and_exb;
} PML4_entry, PDP_entry, PD_entry, PT_entry;
#pragma pack(pop)

void k_initialize_page_tables(void);
void k_set_page_entry(PT_entry* pst_entry, uint32_t attr_and_lower, uint32_t upper_and_exb, uint32_t lower_flags, uint32_t upper_flags);
