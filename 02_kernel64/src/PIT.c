#include "PIT.h"
#include "types.h"
#include "k_inout.h"

void k_init_PIT(uint16_t count, BOOL is_periodic) {
    
    k_outb(PIT_PORT_CONTROL, is_periodic ? PIT_COUNTER0_PERI : PIT_COUNTER0_ONCE);

    // LSBMSBRW mode is on

    k_outb(PIT_PORT_COUNTER0, count);
    k_outb(PIT_PORT_COUNTER0, count >> 8);
}


uint16_t k_read_counter0(void) {
    k_outb(PIT_PORT_CONTROL, PIT_COUNTER0_LATCH); // LATCH MODE is on, can read

    uint8_t lower = k_inb(PIT_PORT_COUNTER0);
    uint8_t upper = k_inb(PIT_PORT_COUNTER0);

    return (uint16_t) (((uint16_t) upper << 8) | lower);
}

void k_wait_using_direct_PIT(uint16_t count) {
    k_init_PIT(0, TRUE); // 0x10000 and periodic

    uint16_t last_count = k_read_counter0();


    while( ((last_count - k_read_counter0()) & 0xFFFF) < count);
}
