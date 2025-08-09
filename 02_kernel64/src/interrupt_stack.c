#include "types.h"
#include "interrupt_helper.h"

uint8_t cli_stack = 1; // why doing cli twice gives me interrupt 9? 
// we enter the kernel64 with cli is on, so we must start with 1.

BOOL reenable_on_exit = TRUE;
// interrupts will make IF=0 witout our push,pop cli. so we need to check whether it was on or off.
// initialize with TRUE, because the first pop_cli will be enabling the interrupt
void push_cli(void) {
    if(cli_stack++ == 0) {
        reenable_on_exit = (k_read_RFLAGS() & (1ULL << 9)) != 0;
        k_disable_interrupt();
    }
}

void pop_cli(void) {
    if (--cli_stack == 0) {
        if (reenable_on_exit) k_enable_interrupt();
        reenable_on_exit = FALSE;
    }
}