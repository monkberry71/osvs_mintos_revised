#pragma once

#include "types.h"

// Define IO port
#define PIC_MASTER_PORT1 0x20
#define PIC_MASTER_PORT2 0x21
#define PIC_SLAVE_PORT1 0xA0
#define PIC_SLAVE_PORT2 0xA1

// the offset where our PIC oriented interrupts start in IDT table.
#define PIC_IRQ_START_VECTOR 0x20

void k_init_PIC(void);
void k_mask_PIC_interrupt(uint16_t IRQ_bitmask);
void k_send_EOI_to_PIC(int IRQ_num);

