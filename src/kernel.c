// src/kernel.c

void kernel_main() {
    volatile unsigned short *vga = (volatile unsigned short *)0xB8000;
    const char *msg = "Hello from kernel";

    for (int i = 0; msg[i] != '\0'; i++) {
        vga[i] = (0x0F << 8) | msg[i];
    }

    while (1) {
        __asm__ volatile ("hlt");
    }
}