import sys
import os
import math

# This script edits bootloader's sector length bytes.

def patch_kernel(file_path):
    size_bytes = os.path.getsize(file_path)
    sectors = math.ceil(size_bytes / 512)

    with open(file_path, "ab") as f:
        f.write(bytes([0 for _ in range(sectors*512 - size_bytes)]))

    return sectors

def patch_bootloader(bootloader_path, kernel_path):
    sectors = patch_kernel(kernel_path)

    if not (1 <= sectors <= 255):
        raise ValueError(f"Kernel too large! {sectors} sectors won't fit in 1 byte.")

    with open(bootloader_path, "r+b") as f:
        f.seek(5)  # Offset 2 = 3rd byte
        f.write(bytes([sectors]))

    print(f"Patched '{bootloader_path}' with kernel sector count = {sectors} (from '{kernel_path}')")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 write_sector_count.py <bootloader.bin> <kernel.img>")
        sys.exit(1)

    bootloader = sys.argv[1]
    kernel = sys.argv[2]

    patch_bootloader(bootloader, kernel)