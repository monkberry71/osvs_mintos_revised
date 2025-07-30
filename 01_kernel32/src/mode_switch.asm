[BITS 32]

global k_read_CPUID, k_switch_kernel64

SECTION .text

; Return CPUID
; PARAM : uint32_t dwEAX, uint32_t* pdwEAX, pdwEBX, pdwECX, pdwEDX
k_read_CPUID:
    push ebp,
    mov ebp, esp
    push eax
    push ebx
    push ecx
    push edx
    push esi

    mov eax, dword [ebp + 8] ; load eax from the parameter 
    cpuid;

    ;; let's move all returned values to C variables
    ; eax
    mov esi, dword [ebp + 12]
    mov dword [esi], eax

    ; ebx
    mov esi, dword [ebp + 16]
    mov dword [esi], ebx

    ; ecx
    mov esi, dword [ebp + 20]
    mov dword [esi], ecx

    ; edx
    mov esi, dword [ebp + 24]
    mov dword [esi], edx

    pop esi
    pop edx
    pop ecx 
    pop ebx
    pop eax
    mov esp, ebp
    pop ebp
    ret

k_switch_kernel64:
    mov eax, cr4 
    or eax, 0x20; PAE bit set
    mov cr4, eax

    mov eax, 0x100000 ; our page table
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr ; read msr register

    or eax, 0x0100 ; set 64 bit mode

    wrmsr

    mov eax, cr0
    or eax, 0xE0000000 ;; paging on, caching on.
    xor eax, 0x60000000 ;; disable caching for now..
    mov cr0, eax

    jmp 0x08:0x200000

    ; non-reachable
    jmp $;