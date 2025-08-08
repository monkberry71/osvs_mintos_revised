[BITS 64]

SECTION .text

global k_enable_interrupt, k_disable_interrupt, k_read_RFLAGS, k_int_3

k_enable_interrupt:
    sti;
    ret;

k_disable_interrupt:
    cli;
    ret;

k_read_RFLAGS:
    pushfq
    pop rax;
    ret;

k_int_3:
    int 0x03;
    ret;