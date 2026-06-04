ASM = nasm
CC = gcc
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -nostdlib -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld

BUILD_DIR = build
KERNEL = $(BUILD_DIR)/kernel.elf

all: $(KERNEL)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/boot.o: src/boot.asm | $(BUILD_DIR)
	$(ASM) -f elf32 src/boot.asm -o $(BUILD_DIR)/boot.o

$(BUILD_DIR)/kernel.o: src/kernel.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c src/kernel.c -o $(BUILD_DIR)/kernel.o

$(KERNEL): $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o
	$(LD) $(LDFLAGS) $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o -o $(KERNEL)

run: $(KERNEL)
	qemu-system-i386 -kernel $(KERNEL)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean
