[ORG 0x00] ; set the code's starting address. 
[BITS 16] ; tell nasm to assemble 16 bit codes
SECTION .text ; for linker

jmp 0x07c0:start;
TOTAL_SECTOR_COUNT: dw 1024

start:
    ;;;;;;;;;;;;;;;;;;;;;;;; segment init
    mov ax, 0x07c0
    mov ds, ax ; set DS segment regi to 0x07c0

    mov ax, 0xB800
    mov es, ax ; set ES segment regi to 0xB800

    mov ax, 0x00
    mov ss, ax ; set SS segment regi to 0x0000
    mov sp, 0xFFFE
    mov bp, 0xFFFE ; because it is real mode, the sp should point to 0xFFFE, (0xFFFE, 0xFFFF) is one stack element.

    mov si, 0 ; set si to 0
    ; we will use si as a counter
.screen_clear_loop:
    mov byte [es:si], 0
    mov byte [es:si+1], 0x0A ; empty the <si>th text buffer

    add si, 2 ; go to next <si>th buffer

    cmp si, 80 * 25 * 2; width 80, height 25, buffer 2
    jl .screen_clear_loop; if (si - 80 * 25 * 2 < 0), should loop.

    ;;;;; printing
    push MSG_1
    push 0
    push 0
    call .print_msg
    add sp, 6; cdecl style

    push IMAGE_LOADING_MSG
    push 1
    push 0
    call .print_msg
    add sp, 6

    ;;;;; let's read floppy with BIOS
.reset_disk:
    mov ax, 0; service number
    mov dl, 0; floppy number
    int 0x13; bios call

    jc .handle_disk_error

    mov si, 0x1000; 
    mov es, si
    mov bx, 0x0000; BIOS will load the floppy to ES:BX, so we set it 0x1000:0x0000, 0x10000.

    mov di, word [TOTAL_SECTOR_COUNT]; now di is TOTAL_SECTOR_COUNT

.read_data:
    cmp di, 0;
    je .read_end
    sub di, 0x1; while(di != 0), do next

    mov ah, 0x02 ; BIOS service number 2, read sector
    mov al, 0x1 ; how many sector to read == 1
    mov ch, byte [ TRACK_NUMBER ]
    mov cl, byte [ SECTOR_NUMBER ]
    mov dh, byte [ HEAD_NUMBER ]
    mov dl, 0x00; which drive to read 0 == floppy

    int 0x13
    jc .handle_disk_error

    add si, 0x20
    mov es, si; 512 == 0x200, so move ES to next 512 byte

    mov al, byte [ SECTOR_NUMBER ]
    add al, 0x01
    mov byte [ SECTOR_NUMBER ], al
    cmp al, 37
    jl .read_data; if (al < 19), jump to .read_data

    ; now, 18 sectors are all read, head and sector number need to be resetted.
    xor byte [ HEAD_NUMBER ], 0x01
    mov byte [ SECTOR_NUMBER ], 0x01

    cmp byte [ HEAD_NUMBER ], 0x00;
    jne .read_data; if HEAD_NUMBER isn't 0, it means it is 1, and it again means that we should read again.

    add byte [ TRACK_NUMBER ], 0x01;
    jmp .read_data

.read_end:
    push LOAD_COMPLETE_MSG
    push 1
    push 20
    call .print_msg
    add sp, 6


    jmp 0x1000:0x0000;

.handle_disk_error:
    ;; BIOS error has occured.
    push DISK_ERR_MSG
    push 1
    push 20
    call .print_msg

    jmp $

;;; function area ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.print_msg:
    push bp; bp is callee saved register
    mov bp, sp; bp now points to former bp.

    ; bp + 0 is former bp, bp + 1 * 2 is return address, bp + x * 2 is (x-1)th parameter

    push es
    push si
    push di
    push ax
    push cx
    push dx

    mov ax, 0xB800
    mov es,ax; set EX as video buffer

    ; get line address
    mov ax, word [ bp + 3 * 2]; ax = y
    mov si, 160; 80 * 2, 1 line byte count.
    mul si; ax *= 160
    mov di, ax; di = y * 160

    ; get offset on a line
    mov ax, word [ bp + 2 * 2]; ax = x
    mov si, 2; 1 char byte count.
    mul si; 
    add di, ax; di is now 2(80 * y + x)

    mov si, word [bp+4*2]; si is now pointer to string.

    .msg_loop:
        mov cl, byte [si]; 1 char 
        cmp cl, 0;
        je .msg_end; if the char is 0, end

        ;if not, let's print it
        mov byte[es:di], cl;
        
        add si, 1; next char
        add di, 2; next print location

        jmp .msg_loop

    .msg_end:
        pop dx
        pop cx
        pop ax
        pop di
        pop si
        pop es
        mov sp, bp
        pop bp
        ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


MSG_1: db 'mint64 boot loader starting...', 0
DISK_ERR_MSG: db 'BIOS cannot read from the disk.', 0
IMAGE_LOADING_MSG: db 'OS Image loading...', 0
LOAD_COMPLETE_MSG: db 'OS loading complete!', 0

SECTOR_NUMBER: db 0x02
HEAD_NUMBER: db 0x00
TRACK_NUMBER: db 0x00

times 510 - ( $- $$) db 0x00
db 0x55
db 0xAA