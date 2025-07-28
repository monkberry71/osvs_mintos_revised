#include "page.h"

void k_initialize_page_tables(void) {
    PML4_entry* PML4_entry_in_use;
    PDP_entry* PDP_entry_in_use;
    PD_entry* PD_entry_in_use;
    uint32_t mapping_addr;

    // PML4 Table Creation
    // All entries are 0, except the first one.
    PML4_entry_in_use = (void*) 0x100000;
    k_set_page_entry(PML4_entry_in_use, 0x00, 0x101000, PAGE_FLAGS_DEFAULT, 0); // at 0x101000, there is our THE PDP table.
    for(int i=1; i < PAGE_MAX_ENTRY_COUNT; i++) k_set_page_entry(PML4_entry_in_use + i, 0,0,0,0);

    // PDP Table Creation
    // 64 entries.
    PDP_entry_in_use = (void*) 0x101000;
    for(int i=0; i<64; i++) {
        k_set_page_entry(PDP_entry_in_use + i, 0, 0x102000 + (i * PAGE_TABLE_SIZE), PAGE_FLAGS_DEFAULT, 0);
    }
    for(int i=64; i<PAGE_MAX_ENTRY_COUNT; i++) {
        k_set_page_entry(PDP_entry_in_use + i, 0, 0x102000 + (i * PAGE_TABLE_SIZE), 0, 0);
    }

    // Page directory table Creation
    // 1GB per a table
    PD_entry_in_use = (void*) 0x102000;
    mapping_addr = 0;
    for (int i=0; i < PAGE_MAX_ENTRY_COUNT * 64; i++) {
        k_set_page_entry( PD_entry_in_use + i, (i * (PAGE_DEFAULT_SIZE >> 20)) >> 12, mapping_addr, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0);
        mapping_addr += PAGE_DEFAULT_SIZE;
        // ith page directory entry : 0 ~ 512 * 64 (512 per table, and there are 64 table).
        // ith page directory entry should have a base address of i * PAGE_DEFAULT_SIZE 
        // lower 32 bit of (i * PAGE_DEFAULT_SIZE) is mapping_addr == (i * PAGE_DEFAULT_SIZE) % 2^32. (mapping_addr is 32bit uint, so there is modular hidden, 
        // so mapping_addr == (i * PAGE_DEFAULT_SIZE)
        // upper 32 bit of (i * PAGE_DEFAULT_SIZE) >> 32, which it is. However, we first make a PAGE_DEFAULT_SIZE to MB unit, which is 2.
    }

}

void k_set_page_entry(PT_entry *entry_in_use, uint32_t upper_addr, uint32_t lower_addr, uint32_t lower_flag, uint32_t upper_flag) {
    entry_in_use->attr_and_lower_base_addr = lower_addr | lower_flag;
    entry_in_use->upper_base_addr_and_exb = upper_addr & 0xFF | upper_flag;
}