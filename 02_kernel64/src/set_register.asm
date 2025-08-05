[BITS 64]

global k_load_GDTR, k_load_TR, k_load_IDTR

SECTION .text

k_load_GDTR:
    lgdt [rdi]
    ret

k_load_TR:
    ltr di
    ret

k_load_IDTR:
    lidt [ rdi ]
    ret