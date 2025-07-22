[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x1000:start

SECTOR_COUNT: dw 0x0000
C_WHOLE_SECTOR_COUNT equ 1024 ; it is an NASM constant, not a label for address

start:
    mov ax, cs
    mov ds, ax ; set DS as 0x1000

    mov ax, 0xB800
    mov es, ax ; set ES as 0xB800

; SECTOR-WISE code
%assign i 0; it is an NASM variable, not a label for address
%rep C_WHOLE_SECTOR_COUNT
    %assign i i+1

    mov ax, 2
    mul word [SECTOR_COUNT]; ax is now SECTOR_COUNT * 2
    mov si, ax ; si is now SECTOR_COUNT * 2 too.

    mov byte [es:si + (160 * 2)], '0' + (i % 10) ; si is SECTOR_COUNT * 2 (index), and we put (i % 10) on screen
    add word [SECTOR_COUNT], 1

    %if i == C_WHOLE_SECTOR_COUNT
        jmp $; if it is a last sector, infinite loop starts.
    %else
        jmp (0x1000 + i * 0x20):0x0000; if not, jump to next sector
    %endif

    times (510 - ($ - $$) % 512) db 0x00;
    dw i;
%endrep


