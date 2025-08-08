#include "types.h"
#include "interrupt_helper.h"

uint8_t cli_stack = 1; // why doing cli twice gives me interrupt 9?

void push_cli(void) {
    if(cli_stack++ == 0) k_disable_interrupt();
}

void pop_cli(void) {
    if (--cli_stack == 0) k_enable_interrupt();
}