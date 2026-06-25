// src/kernel.c

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH   80
#define VGA_HEIGHT  25

static volatile uint16_t *const VGA_BUFFER = (volatile uint16_t *)0xB8000;

enum vga_color {
    VGA_BLACK       = 0,
    VGA_BLUE        = 1,
    VGA_GREN        = 2,
    VGA_CYAN        = 3,
    VGA_RED         = 4,
    VGA_MAGENTA     = 5,
    VGA_BROWN       = 6,
    VGA_LIGHT_GREY  = 7,
    VGA_DARK_GREY   = 8,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN  = 11,
    VGA_LIGHT_RED   = 12,
    VGA_LIGHT_MAGENTA   = 13,
    VGA_LIGHT_BROWN = 14,
    VGA_WHITE       = 15,
};

static size_t term_row = 0;
static size_t term_col = 0;
static uint8_t term_color = 0;

static inline uint8_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

static void terminal_clear(void) {
    for (size_t y = 0; y < VGA_WIDTH; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(' ', term_color);
        }
    }

    term_row = 0;
    term_col = 0;
}

static void terminal_scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[(y - 1) * VGA_WIDTH + x] = VGA_BUFFER[y * VGA_WIDTH + x];
        }
    }

    for (size_t x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', term_color);
    }

    if (term_row > 0) {
        term_row--;
    }
}

static void terminal_putchar(char c) {
    if (c == '\n') {
        term_col = 0;
        term_row++;

        if (term_row >= VGA_HEIGHT) {
            terminal_scroll();
        }

        return;
    }

    if (c == '\r') {
        term_col = 0;
        return;
    }

    VGA_BUFFER[term_row * VGA_WIDTH + term_col] = vga_entry(c, term_color);
    term_col++;

    if (term_col >= VGA_WIDTH) {
        term_col = 0;
        term_row++;

        if (term_row >= VGA_HEIGHT) {
            terminal_scroll();
        }
    }
}

static void terminal_write(const char *s) {
    for (size_t i = 0; s[i] != '\0'; i++) {
        terminal_putchar(s[i]);
    }
}

static void panic(const char *msg) {
    term_color = vga_color(VGA_WHITE, VGA_RED);
    terminal_write("\nKERNEL PANIC: ");
    terminal_write(msg);

    while(1) {
        __asm__ volatile ("clil hlt;");
    }
}

void kernel_main() {
    term_color = vga_color(VGA_LIGHT_GREY, VGA_BLACK);
    terminal_clear();

    terminal_write("sakukernel booted!\n");
    terminal_write("terminal: ok\n");
    terminal_write("newline: ok\n");
    terminal_write("scroll: ok\n");

    // panic("test panic");

    while (1) {
        __asm__ volatile ("hlt");
    }
}
