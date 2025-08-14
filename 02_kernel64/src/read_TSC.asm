[BITS 64]

SECTION .text

global k_read_TSC

k_read_TSC:
    push rdx

    rdtsc; RDX:RAX = time stamp

    shl rdx, 32
    or rax, rdx

    pop rdx
    ret;

