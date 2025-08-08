.PHONY: clean all
all: boot_loader disk.img misc

boot_loader:
	@echo "=== Building boot_loader ==="
	make -C 00_boot_loader 
# 	-C : go to the directry and execute Makefile there
	@echo "=== Build complete ==="

kernel32:
	@echo "=== Building kernel32 ==="
	make -C 01_kernel32
	@echo "=== Build complete ==="

kernel64:
	@echo "=== Building kernel64 ==="
	make -C 02_kernel64
	@echo "=== Build complete ==="


misc:
	@echo "=== Building Mischellaneous ==="
	make -C 05_misc
	@echo "=== Build Complete ==="

disk.img: boot_loader kernel32 kernel64
	@echo "=== Building disk image ==="
# 	cp 00_boot_loader/boot_loader.bin disk.img
# 	cat 00_boot_loader/boot_loader.bin 01_kernel32/kernel32.bin > disk.img
	python3 ./04_util/sector_count_editor.py ./00_boot_loader/boot_loader.bin 01_kernel32/kernel32.bin 02_kernel64/kernel64.bin

	@echo "=== Build complete ==="

clean:
	make -C 00_boot_loader clean
	make -C 01_kernel32 clean
	make -C 02_kernel64 clean
	rm -f disk.img

test: all
	qemu-system-x86_64 -m 64 -fda disk.img -boot a -M pc -d int,cpu_reset,unimp -D qemu.log -no-reboot
# 	-m 64 : memory 64mb
#	-fda disk.img :  floppy img
#	-boot a :  boot from floppy
# 	-M pc : emulate personal computer

test-usb: all
# 	qemu-system-x86_64 -m 64M -usb -drive if=none,id=flp,format=raw,file=disk.img -device usb-storage,drive=flp -boot order=a
	qemu-system-x86_64 -m 64M -machine pc,usb=on -drive if=none,id=usbdisk,format=raw,file=disk.img -device usb-storage,drive=usbdisk -boot order=c


test-nox: all
	qemu-system-x86_64 -m 64 -fda disk.img -boot a -M pc -nographic

gdb: all
	qemu-system-x86_64 -m 64 -fda disk.img -boot a -M pc -S -gdb tcp::1234 -no-reboot 

load-usb: all
	sudo dd if=disk.img of=/dev/sda bs=4M conv=fsync
	sudo sync
	echo ',,83,*' | sudo sfdisk /dev/sda