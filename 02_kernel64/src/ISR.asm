[BITS 64]

SECTION .text

extern k_common_exception_handler, k_common_interrupt_handler, k_keyboard_handler

%macro K_SAVE_CONTEXT 0
    push    rbp
    mov     rbp, rsp

    push    rax
    push    rcx
    push    rdx
    push    rbx
    push    rdi
    push    rsi
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15

    mov ax, ds
    push rax

    mov ax, es
    push rax
    
    push fs
    push gs

    ; change segment to kernel one
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
%endmacro

%macro K_LOAD_CONTEXT 0
    pop gs
    pop fs

    pop rax
    mov es, ax

    pop rax
    mov ds, ax

    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdi
    pop     rsi
    pop     rbx
    pop     rdx
    pop     rcx
    pop     rax
    mov     rsp, rbp
    pop     rbp
%endmacro

; ------------------------------------------------------------
; handler: define an IDT stub for vector %1
; ------------------------------------------------------------
%macro handler 1
global k_interrupt_%1_handler

k_interrupt_%1_handler:
    K_SAVE_CONTEXT

    mov     rdi, %1

    ; 1) Keyboard IRQ (vector 33)
%if %1 == 33
    call    k_keyboard_handler

    ; 2) Other hardware IRQs (vectors 32+ except 33)
%elif %1 >= 32
    call    k_common_interrupt_handler

    ; 3) Exceptions that pushed an error code
%elif %1 == 8  or %1 == 10 or %1 == 11 or %1 == 12 or \
    %1 == 13 or %1 == 14 or %1 == 17
    mov     rsi, qword [rbp+8]
    call    k_common_exception_handler

    ; 4) All other CPU exceptions (no error code)
%else
    call    k_common_exception_handler
%endif

    K_LOAD_CONTEXT
    iretq
%endmacro

; Instantiate handlers 0 through 48
%assign vec 0
%rep 49
    handler vec
    %assign vec vec+1
%endrep
