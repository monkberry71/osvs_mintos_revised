[ORG 0x00]
[BITS 16]

; This is a protected mode (32bit) mode kernel's entry point.
; Why this file is .s? -> .asm files need to be built different from this file.
SECTION .text

start:
    mov ax, 0x1000
    mov ds, ax
    mov es, ax; set DS and ES register

    ;;; A20 GATE ACTIVATION
    mov ax, 0x2401 ; A20 gate service number
    int 0x15; BIOS interrupt

    jc .A20_ERROR
    jmp .A20_SUCCESS

    .A20_ERROR:
    in al, 0x92
    or al, 0x02
    and al, 0xFE
    out 0x92, al
    
    .A20_SUCCESS:
    cli ; disable interrupt
    lgdt [GDTR]; set GDT

    mov eax, 0x4000003B; 
    mov cr0, eax; again if you have protected mode supporting cpu, you can access eax in real mode too, but it is done by assembler to manage this. set cr0 register to go protected mode.

    ;;;; protected mode enabled
    jmp dword 0x18: (.protected_mode - $$ + 0x10000) ; 0x08 is for our segment selector, we cannot just jump to .protected_mode label because our 0x08 segment descriptor's base address is 0.

[BITS 32]
.protected_mode:
    mov ax, 0x20
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax ; second descriptor goes all to them.

    mov ss, ax;
    mov esp, 0xFFFC
    mov ebp, 0xFFFC; let's align 4 byte.

    push (SWITCH_SUCCESS_MSG - $$ + 0x10000); our code is assembled as starting from 0, but our code is actually loaded at 0x10000. so we need to add 0x10000 to our labels. We can circumvent this by setting the appropriate segmentation, but we don't do that and just add 0x10000.
    push 2
    push 0
    call PRINT_MSG
    add esp, 12 ; cdecl!

    jmp dword 0x18:0x10200

;;;;;;;;;;;; function area
PRINT_MSG:
    push ebp
    mov ebp, esp
    push esi
    push edi 
    push eax
    push ecx
    push edx

    ; ebp is now former ebp, ebp + 4 is return addr, ebp + 4 * (x + 1) is xth argument.

    mov eax, dword [ebp + 4 * (2 + 1)]
    mov esi, 160
    mul esi
    mov edi, eax; edi is now 160 * y, remember that 160 is width * 2

    mov eax, dword [ebp + 4 * (1 + 1)]
    mov esi, 2
    mul esi
    add edi, eax; edi is now 160*y + 2*x == 2(80y+x)

    mov esi, dword [ebp + 4 * (3 + 1)]

    .msg_loop:
        mov cl, byte [esi]
        cmp cl, 0
        je .msg_end; if cl == 0, end loop.

        mov byte [edi + 0xB8000], cl
        add esi, 1
        add edi, 2

        jmp .msg_loop
    
    .msg_end:

    pop edx
    pop ecx
    pop eax
    pop edi
    pop esi
    pop ebp
    ret
;;;;;

align 8, db 0 ; assure that next label is starting at 8 byte aligned addr

dw 0x1234 ; GDTR is 12 byte. let's add 4 zero bytes to align it to 8 bytes.

GDTR:
    dw GDT_END - GDT ; GDT size, let's say GDT is 0 and GDT_END is 2, then the right size is 2.
    dd (GDT-$$+0x10000) ; GDT address

GDT:
    NULL_Descriptor:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00
        db 0x00
        db 0x00
    IA_32e_CODE_Descriptor:
        dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x9A ; execute / read
        db 0xAF
        db 0x00
    IA_32e_DATA_Descriptor:
        dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x92
        db 0xAF
        db 0x00
    CODE_Descriptor:
        dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x9A
        db 0xCf
        db 0x00
    DATA_Descriptor:
        dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x92
        db 0xCF
        db 0x00
GDT_END:
SWITCH_SUCCESS_MSG: db 'Switching to protected mode successed.', 0

times 512 - ($ - $$) db 0x00

