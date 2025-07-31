NASM64 = nasm -f elf64
GCC64 = gcc -c -m64 -ffreestanding -fno-pie -fno-pic
LD64 = ld -melf_x86_64 -T ../binary_amd64.x -nostdlib -e main -Ttext 0x200000
OBJCOPY64 = objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

OBJ_DIR = temp
SRC_DIR = src

all: prepare kernel64.bin

prepare:
	mkdir -p $(OBJ_DIR)

dep:
	@echo "=== Make Dependancy ==="
	make -C $(OBJ_DIR) -f ../Makefile internal_dep
	@echo "=== Done ==="

execute_internal_build: dep
	make -C $(OBJ_DIR) -f ../Makefile kernel64.elf

kernel64.bin: execute_internal_build
	$(OBJCOPY64) $(OBJ_DIR)/kernel64.elf $@

clean:
	rm -f *.bin 
	rm -f $(OBJ_DIR)/*.*

############################

ENTRY_SRC_FILE = ../$(SRC_DIR)/entry_point.s
ENTRY_OBJ_FILE = entry_point.o 

C_SRC_FILES = $(wildcard ../$(SRC_DIR)/*.c)
ASM_SRC_FILES = $(wildcard ../$(SRC_DIR)/*.asm)
C_OBJ_FILES = $(notdir $(patsubst %.c,%.o,$(C_SRC_FILES)))
ASM_OBJ_FILES = $(notdir $(patsubst %.asm,%.o,$(ASM_SRC_FILES)))

debug:
	@echo "ESF=$(ENTRY_SRC_FILE)"

$(ENTRY_OBJ_FILE): $(ENTRY_SRC_FILE)
	$(NASM64) -o $@ $<

%.o: ../$(SRC_DIR)/%.c 
	$(GCC64) -c $<

%.o: ../$(SRC_DIR)/%.asm
	$(NASM64) -o $@ $<

internal_dep:
	$(GCC64) -MM $(C_SRC_FILES) > dependancy.dep

kernel64.elf: $(ENTRY_OBJ_FILE) $(C_OBJ_FILES) $(ASM_OBJ_FILES)
	$(LD64) -o $@ $^

-include dependancy.dep