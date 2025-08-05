#pragma once

#include "types.h"

// === GDT Macros ================================
#define GDT_TYPES_CODE 0x0A
#define GDT_TYPES_DATA 0x02
#define GDT_TYPES_TSS  0x09
#define GDT_FLAGS_LOWER_S 0x10
#define GDT_FLAGS_LOWER_DPL0 0x00
#define GDT_FLAGS_LOWER_DPL1 0x20
#define GDT_FLAGS_LOWER_DPL2 0x40
#define GDT_FLAGS_LOWER_DPL3 0x60
#define GDT_FLAGS_LOWER_P 0x80
#define GDT_FLAGS_UPPER_L 0x20
#define GDT_FLAGS_UPPER_DB 0x40
#define GDT_FLAGS_UPPER_G 0x80 // these are basic blocks

#define GDT_FLAGS_LOWER_KERNEL_CODE ( GDT_TYPES_CODE | GDT_FLAGS_LOWER_S | GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P ) // it is a code segment descriptor, so S need to be set
#define GDT_FLAGS_LOWER_KERNEL_DATA ( GDT_TYPES_DATA | GDT_FLAGS_LOWER_S | GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P )
#define GDT_FLAGS_LOWER_TSS ( GDT_TYPES_TSS | GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P ) 
// it is a TSS, so it is a system descriptor
#define GDT_FLAGS_LOWER_USER_CODE ( GDT_TYPES_CODE | GDT_FLAGS_LOWER_S | GDT_FLAGS_LOWER_DPL3 | GDT_FLAGS_LOWER_P )
#define GDT_FLAGS_LOWER_USER_DATA ( GDT_TYPES_DATA | GDT_FLAGS_LOWER_S | GDT_FLAGS_LOWER_DPL3 | GDT_FLAGS_LOWER_P )

#define GDT_FLAGS_UPPER_CODE ( GDT_FLAGS_UPPER_G | GDT_FLAGS_UPPER_L ) // L for long 64
#define GDT_FLAGS_UPPER_DATA ( GDT_FLAGS_UPPER_G | GDT_FLAGS_UPPER_L )
#define GDT_FLAGS_UPPER_TSS 0

#define GDT_KERNEL_CODE_SEGMENT_OFFSET 0x08
#define GDT_KERNEL_DATA_SEGMENT_OFFSET 0x10
#define GDT_TSS_SEGMENT_OFFSET 0x18

#define GDTR_ADDR 0x142000
#define GDT_MAX_ENTRY_8_COUNT 3 // NULL desc, KERNEL CODE, KERNEL DATA
#define GDT_MAX_ENTRY_16_COUNT 1 // TSS
#define GDT_SIZE ( (sizeof(gdt_entry_8) * GDT_MAX_ENTRY_8_COUNT) + (sizeof(gdt_entry_16) * GDT_MAX_ENTRY_16_COUNT) )
#define TSS_SEGMENT_SIZE ( sizeof(tss_segment) )

// === IDT Macros ========================
#define IDT_TYPE_INTERRUPT 0x0E
#define IDT_TYPE_TRAP 0x0F
#define IDT_FLAGS_DPL0 0x00
#define IDT_FLAGS_DPL1 0x20
#define IDT_FLAGS_DPL2 0x40
#define IDT_FLAGS_DPL3 0x60
#define IDT_FLAGS_P 0x80
#define IDT_FLAGS_IST0 0
#define IDT_FLAGS_IST1 1

#define IDT_FLAGS_KERNEL ( IDT_FLAGS_DPL0 | IDT_FLAGS_P )
#define IDT_FLAGS_USER ( IDT_FLAGS_DPL3 | IDT_FLAGS_P )

#define IDT_MAX_ENTRY_COUNT 100
#define IDTR_ADDR (GDTR_ADDR + sizeof(gdtr) + GDT_SIZE + TSS_SEGMENT_SIZE)
#define IDT_ADDR (IDTR_ADDR + sizeof(idtr))
#define IDT_SIZE ( IDT_MAX_ENTRY_COUNT * sizeof(idt_entry))

#define IST_ADDR 0x700000
#define IST_SIZE 0x100000

#pragma pack(push,1)

typedef struct GDTR_STR {
    uint16_t limit;
    uint64_t base_addr;
    uint16_t padding;
    uint32_t padding2;
} gdtr, idtr;

typedef struct GDT_ENTRY_8 {
    uint16_t lower_limit;
    uint16_t lower_base_addr;
    uint8_t upper_base_addr1;
    uint8_t type_and_lower_flag;
    uint8_t upper_limit_and_upper_flag;
    uint8_t upper_base_addr2;
} gdt_entry_8;

typedef struct GDT_ENRTY_16 {
    uint16_t lower_limit;
    uint16_t lower_base_addr;
    uint8_t middle_base_addr1;
    uint8_t type_and_lower_flag;
    uint8_t upper_limit_and_upper_flag;
    uint8_t middle_base_addr2;
    uint32_t upper_base_addr;
    uint32_t reserved;
} gdt_entry_16;

typedef struct TSS_DATA_STRUCT {
    uint32_t reserved1;
    uint64_t rsp[3];
    uint64_t reserved2;
    uint64_t ist[7];
    uint64_t reserved3;
    uint16_t reserved4;
    uint16_t iomap;
} tss_segment;

typedef struct IDT_ENTRY_STRUCT {
    uint16_t lower_base_addr;
    uint16_t segment_selector;
    uint8_t IST;
    uint8_t type_and_flags;
    uint16_t middle_base_addr;
    uint32_t upper_base_addr;
    uint32_t reserved;
} idt_entry;

#pragma pack (pop)
/**
 * @brief  Configure an 8-byte GDT entry (code or data descriptor).
 * @param  entry         Pointer to the GDT entry to initialize.
 * @param  base_addr     Base address of the segment.
 * @param  limit         Segment limit (byte size minus one).
 * @param  upper_flags   Upper flags (granularity, size, etc.).
 * @param  lower_flags   Lower flags (present, DPL).
 * @param  type          Descriptor type (code or data).
 */
void k_set_GDT_entry8(
    gdt_entry_8* entry,
    uint32_t     base_addr,
    uint32_t     limit,
    uint8_t      upper_flags,
    uint8_t      lower_flags,
    uint8_t      type
);

/**
 * @brief  Configure a 16-byte GDT entry (TSS descriptor for 64-bit mode).
 * @param  entry         Pointer to the GDT entry to initialize.
 * @param  base_addr     Base address of the TSS structure.
 * @param  limit         Size of the TSS segment (byte size minus one).
 * @param  upper_flags   Upper flags (granularity, etc.).
 * @param  lower_flags   Lower flags (present, DPL).
 * @param  type          Descriptor type (TSS).
 */
void k_set_GDT_entry16(
    gdt_entry_16* entry,
    uint64_t      base_addr,
    uint32_t      limit,
    uint8_t       upper_flags,
    uint8_t       lower_flags,
    uint8_t       type
);

/**
 * @brief  Initialize the Task-State Segment (TSS) fields.
 * @param  TSS           Pointer to the TSS structure to clear and set up.
 */
void k_init_TSS_segment(
    tss_segment* TSS
);

/**
 * @brief  Set up the Global Descriptor Table (GDT) and load the TSS.
 * @details Allocates entries for null, code, data, and TSS, then initializes the TSS and loads it.
 */
void k_init_GDT_TSS(void);

/**
 * @brief  Initialize the Interrupt Descriptor Table (IDT) with dummy handlers.
 * @details Sets all entries to a dummy handler using IST1 and the kernel code segment.
 */
void k_init_IDT_table(void);

/**
 * @brief  Configure an IDT entry (interrupt gate) for a specific vector.
 * @param  entry         Pointer to the IDT entry to initialize.
 * @param  handler       Address of the interrupt handler function.
 * @param  selector      Code segment selector for the handler.
 * @param  IST           Interrupt Stack Table index (0 for none).
 * @param  flags         Descriptor flags (DPL, present).
 * @param  type          Gate type (interrupt or trap).
 */
void k_set_IDT_entry(
    idt_entry* entry,
    void*      handler,
    uint16_t   selector,
    uint8_t    IST,
    uint8_t    flags,
    uint8_t    type
);

/**
 * @brief  Dummy interrupt handler that loops indefinitely.
 * @note   Primarily used as a placeholder during early initialization.
 */
void k_dummy_handler(void);


