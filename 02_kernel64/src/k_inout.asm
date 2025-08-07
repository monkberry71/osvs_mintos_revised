[BITS 64]

SECTION .text;

global k_inb, k_outb

k_inb:
    ;; byte k_inb(w_port_number from_where)
    push rdx ;

    mov rdx, rdi ; save param 1 to rdx
    mov rax, 0
    in al, dx; read from [DX register]th port and save it to al.

    pop rdx;
    ret;

k_outb:
    ;; k_outb(w_port number to_where, byte what_to_write)
    push rdx;
    push rax;

    mov rdx, rdi
    mov rax, rsi;
    out dx, al; 

    pop rax
    pop rdx
    ret;
 