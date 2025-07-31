import sys
import os
import math

# This script edits bootloader's sector length bytes.

def pad_kernel(file_path):
    size_bytes = os.path.getsize(file_path)
    sectors = math.ceil(size_bytes / 512)

    with open(file_path, "ab") as f:
        f.write(bytes([0 for _ in range(sectors*512 - size_bytes)]))

    return sectors

def patch_bootloader(bootloader_path, sectors, offset):
    if not (1 <= sectors <= 255):
        raise ValueError(f"Kernel too large! {sectors} sectors won't fit in 1 byte.")

    with open(bootloader_path, "r+b") as f:
        f.seek(offset)  # Offset 2 = 3rd byte
        f.write(bytes([sectors]))

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python3 write_sector_count.py <bootloader.bin> <kernel32.img> <kernel64.img>")
        sys.exit(1)

    bootloader_path = sys.argv[1]
    kernel32_path   = sys.argv[2]
    kernel64_path   = sys.argv[3]

    # pad each kernel to a multiple of 512 bytes and compute sector counts
    kernel32_sector_count = pad_kernel(kernel32_path)
    kernel64_sector_count = pad_kernel(kernel64_path)
    whole_sector_count    = kernel32_sector_count + kernel64_sector_count

    # patch the sector-count fields in the bootloader
    patch_bootloader(bootloader_path, whole_sector_count     , offset=5)
    patch_bootloader(bootloader_path, kernel32_sector_count  , offset=7)

    # now build the raw disk image by concatenating:
    #   [bootloader (512B)] [kernel32 (N·512B)] [kernel64 (M·512B)]
    disk_img_path = os.path.abspath(os.path.join(os.path.dirname(bootloader_path), "..", "disk.img"))
    with open(disk_img_path, "wb") as disk:
        for path in (bootloader_path, kernel32_path, kernel64_path):
            with open(path, "rb") as f:
                disk.write(f.read())

    print(f"Created disk image: {disk_img_path}")