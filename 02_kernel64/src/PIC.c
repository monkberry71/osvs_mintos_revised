#include "PIC.h"
#include "k_inout.h"

void k_init_PIC(void) {
    // Init master PIC by ICW1
    // ICW1, IC4 bit = 1
    k_outb(PIC_MASTER_PORT1, 0x11);

    //ICW2, vector table offset
    k_outb(PIC_MASTER_PORT2, PIC_IRQ_START_VECTOR);

    //ICW3, where is slave pin?
    k_outb(PIC_MASTER_PORT2, 0x04); // bitmask 0000 0100 == bit 2

    //ICW4, every bit is 0 but uPM
    k_outb(PIC_MASTER_PORT2, 0x01);


    // slave
    k_outb(PIC_SLAVE_PORT1, 0x11);
    k_outb(PIC_SLAVE_PORT2, PIC_IRQ_START_VECTOR + 8);
    k_outb(PIC_SLAVE_PORT2, 0x02); // where is the master
    k_outb(PIC_SLAVE_PORT2, 0x01);

}

void k_mask_PIC_interrupt(uint16_t IRQ_bitmask) {
    k_outb(PIC_MASTER_PORT2, (uint8_t) IRQ_bitmask);
    k_outb(PIC_SLAVE_PORT2, (uint8_t) (IRQ_bitmask >> 8));
}

void k_send_EOI_to_PIC(int IRQ_num) {
    k_outb(PIC_MASTER_PORT1, 0x20);

    // if it is slave's IRQ, send to both
    if(IRQ_num > 8) 
        k_outb(PIC_SLAVE_PORT2, 0x20);
}