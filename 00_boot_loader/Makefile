.PHONY: all clean
all: boot_loader.bin

boot_loader.bin: boot_loader.asm
	nasm -o boot_loader.bin boot_loader.asm

clean:
	rm -f boot_loader.bin