.PHONY: clean prepare dep execute_internal_build internal_dep

NASM = nasm
GCC32 = gcc -c -m32 -ffreestanding
LD32 = ld -melf_i386 -T ../elf_i386.x -nostdlib -e main -Ttext 0x10200
OBJCOPY32 = objcopy -j .text -j .data -j .rodata -j .bss -S -O binary
# -S : stript relocation info, -O output format

OBJ_DIR = temp
SRC_DIR = src

all: prepare kernel32.bin

prepare:
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/entry_point.bin: $(SRC_DIR)/entry_point.s
	$(NASM) -o $@ $<

dep:
	@echo "=== Make dependancy files ==="
	make -C $(OBJ_DIR) -f ../Makefile internal_dep
	@echo "=== Dependancy seaching complete ==="

execute_internal_build: dep
	make -C $(OBJ_DIR) -f ../Makefile kernel32.elf

$(OBJ_DIR)/kernel32.elf.bin: execute_internal_build
	$(OBJCOPY32) $(OBJ_DIR)/kernel32.elf $@

kernel32.bin: $(OBJ_DIR)/entry_point.bin $(OBJ_DIR)/kernel32.elf.bin
	cat $^ > $@

clean:
	rm -f *.bin
	rm -f $(OBJ_DIR)/*.*

# Temp dir inside
C_ENTRY_OBJ = main.o
C_SRC_FILES = $(wildcard ../$(SRC_DIR)/*.c)
ASM_SRC_FILES = $(wildcard ../$(SRC_DIR)/*.asm)
C_OBJ_FILES = $(subst main.o, ,$(notdir $(patsubst %.c,%.o,$(C_SRC_FILES)) ))
# $(patsubst %.c,%.o,%(C_SRC_FILES)) gives you $(C_SRC_FILE).o, and you need to exclude main.o, because we want it to go forward, so we manually write them later.
ASM_OBJ_FILES = $(notdir $(patsubst %.asm, %.o, $(ASM_SRC_FILES)))

%.o: ../$(SRC_DIR)/%.c
	$(GCC32) -c $<

%.o: ../$(SRC_DIR)/%.asm
	$(NASM) -f elf32 -o $@ $<

internal_dep:
	$(GCC32) -MM $(C_SRC_FILES) > c_dependency.dep

kernel32.elf: $(C_ENTRY_OBJ) $(C_OBJ_FILES) $(ASM_OBJ_FILES) 
	$(LD32) -o $@ $^
# This is the part where we prepend main.o in front.

-include c_dependency.dep
# include only if it exists.