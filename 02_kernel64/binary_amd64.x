OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

SECTIONS {
    .text 0x200000 : {
        entry_point.o(.text)
        main.o(.text)
        *(.text)
    }
    .= = ALIGN(512);
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(.bss)
    }
    /DISCARD/ : {
        *(*)
    }
}