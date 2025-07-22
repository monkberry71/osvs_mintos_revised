.PHONY: clean all
all: boot_loader disk.img misc

boot_loader:
	@echo "=== Building boot_loader ==="
	make -C 00_boot_loader 
# 	-C : go to the directry and execute Makefile there
	@echo "=== Build complete ==="

misc:
	@echo "=== Building Mischellaneous ==="
	make -C 05_misc
	@echo "=== Build Complete ==="

disk.img: boot_loader misc
	@echo "=== Building disk image"
# 	cp 00_boot_loader/boot_loader.bin disk.img
	cat 00_boot_loader/boot_loader.bin 05_misc/placeholderOS.bin > disk.img
	@echo "=== Build complete ==="

clean:
	make -C 00_boot_loader clean
	rm -f disk.img

test: all
	qemu-system-x86_64 -m 64 -fda disk.img -boot a -M pc 
# 	-m 64 : memory 64mb
#	-fda disk.img :  floppy img
#	-boot a :  boot from floppy
# 	-M pc : emulate personal computer

test-nox: all
	qemu-system-x86_64 -m 64 -fda disk.img -boot a -M pc -nographic

gdb: all
	qemu-system-x86_64 -m 64 -fda disk.img -boot a -M pc -S -gdb tcp::1234