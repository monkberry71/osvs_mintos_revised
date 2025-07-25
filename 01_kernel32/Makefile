all: kernel32.bin

kernel32.bin: src/entry_point.asm
	nasm -o kernel32.bin $<

clean:
	rm -f kernel32.bin