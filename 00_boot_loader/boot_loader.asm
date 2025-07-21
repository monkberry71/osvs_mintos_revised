[ORG 0x00] ; set the code's starting address. 
[BITS 16] ; tell nasm to assemble 16 bit codes
SECTION .text ; for linker

jmp 0x07c0:start;

start:
    mov ax, 0x07c0
    mov ds, ax ; set DS segment regi to 0x07c0

    mov ax, 0xB800
    mov es, ax ; set ES segment regi to 0xB800

    mov si, 0 ; set si to 0

    ; we will use si as a counter
.screen_clear_loop:
    mov byte [es:si], 0
    mov byte [es:si+1], 0x0A ; empty the <si>th text buffer

    add si, 2 ; go to next <si>th buffer

    cmp si, 80 * 25 * 2; width 80, height 25, buffer 2
    jl .screen_clear_loop; if (si - 80 * 25 * 2 < 0), should loop.

    mov si, 0;
    mov di, 0; init
.msg_loop:
    mov cl, byte [si + MSG1]
    cmp cl, 0
    je .msg_end ; if (cl == 0), go to msg_end

    mov byte [es:di], cl ; 0xB800:di to print

    add si, 1 ; 
    add di, 2 ; 1 char + 1 bg = 2 byte

    jmp .msg_loop

.msg_end:
    jmp $ ; $ is current address, so it loops infinitely

MSG1: db 'mint64 boot loader starting...', 0
times 510 - ( $- $$) db 0x00
db 0x55
db 0xAA