#include "descriptor.h"
#include "util.h"
#include "ISR.h"

typedef void (*interrupt_handler_t)(void);

#define X(n) k_interrupt_##n##_handler,

interrupt_handler_t k_interrupt_handlers[] = {
    INTERRUPT_VECTORS
};
#undef X

void k_set_GDT_entry8(gdt_entry_8* entry, uint32_t base_addr, uint32_t limit, uint8_t upper_flags, uint8_t lower_flags, uint8_t type) {
    entry->lower_limit = limit & 0xFFFF;
    entry->lower_base_addr = base_addr & 0xFFFF;
    entry->upper_base_addr1 = (base_addr >> 16) & 0xFF;
    entry->upper_base_addr2 = (base_addr >> 24) & 0xFF;

    entry->type_and_lower_flag = lower_flags | type;
    entry->upper_limit_and_upper_flag = ( (limit >> 16) & 0xFF) | upper_flags;
}

void k_set_GDT_entry16(gdt_entry_16* entry, uint64_t base_addr, uint32_t limit, uint8_t upper_flags, uint8_t lower_flags, uint8_t type) {
    entry->lower_limit = limit & 0xFFFF;
    entry->lower_base_addr = base_addr & 0xFFFF;
    entry->middle_base_addr1 = (base_addr >> 16) & 0xFF;
    entry->middle_base_addr2 = (base_addr >> 24) & 0xFF;
    entry->upper_base_addr = (base_addr >> 32);

    entry->type_and_lower_flag = lower_flags | type;
    entry->upper_limit_and_upper_flag = ( (limit >> 16) & 0xFF) | upper_flags;
    entry->reserved = 0;

}

void k_init_TSS_segment(tss_segment* TSS) {
    k_memset(TSS, 0, TSS_SEGMENT_SIZE);
    TSS->ist[0] = IST_ADDR + IST_SIZE;
    TSS->iomap = 0xFFFF; // it is bigger than TSS limit, so io map is not available.
}

void k_init_GDT_TSS(void) {
    gdtr* GDTR_in_use;
    gdt_entry_8* entry_in_use;
    tss_segment* TSS_in_use;

    GDTR_in_use = (void*) GDTR_ADDR;
    entry_in_use = (void*) (GDTR_ADDR + sizeof(gdtr));
    GDTR_in_use->limit = GDT_SIZE - 1;
    GDTR_in_use->base_addr = (uint64_t) entry_in_use;
    
    TSS_in_use = (void*) ((uint64_t) entry_in_use + GDT_SIZE);

    k_set_GDT_entry8( &(entry_in_use[0]), 0, 0, 0, 0, 0); // NULL Desc
    k_set_GDT_entry8( &(entry_in_use[1]), 0, 0xFFFFF, GDT_FLAGS_UPPER_CODE, GDT_FLAGS_LOWER_KERNEL_CODE, GDT_TYPES_CODE);
    k_set_GDT_entry8( &(entry_in_use[2]), 0, 0xFFFFF, GDT_FLAGS_UPPER_DATA, GDT_FLAGS_LOWER_KERNEL_DATA, GDT_TYPES_DATA);
    k_set_GDT_entry16( (gdt_entry_16*) &(entry_in_use[3]), (uint64_t) TSS_in_use, TSS_SEGMENT_SIZE - 1, GDT_FLAGS_UPPER_TSS, GDT_FLAGS_LOWER_TSS, GDT_TYPES_TSS);

    k_init_TSS_segment(TSS_in_use);
}

// === IDT ===
void k_init_IDT_table(void) {
    idtr* IDTR_in_use;
    idt_entry* entry_in_use;

    IDTR_in_use = (void*) IDTR_ADDR;
    entry_in_use = (void*) (IDTR_ADDR + sizeof(idtr));

    IDTR_in_use->base_addr = (uint64_t) entry_in_use;
    IDTR_in_use->limit = IDT_SIZE - 1;

    for (int i=0; i < 48; i++) {
        // 0~99, all interrupts go to dummy handler
        k_set_IDT_entry( &(entry_in_use[i]), k_interrupt_handlers[i], 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    }

    for(int i=48; i<IDT_MAX_ENTRY_COUNT; i++) {
        k_set_IDT_entry( &(entry_in_use[i]), k_interrupt_handlers[48], 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT);
    }
}

void k_set_IDT_entry(idt_entry* entry, void* handler, uint16_t selector, uint8_t ist, uint8_t flags, uint8_t type) {
    entry->lower_base_addr = (uint64_t) handler & 0xFFFF;
    entry->middle_base_addr = ( (uint64_t) handler >> 16) & 0xFFFF;
    entry->upper_base_addr = ( (uint64_t) handler >> 32);

    entry->segment_selector = selector;
    entry->IST = ist & 0x3;
    entry->type_and_flags = type | flags;
    entry->reserved = 0;
}


void k_dummy_handler(void) {
    // k_print(0,0, "INTERRUPT INCOMMING!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    for(;;);
}