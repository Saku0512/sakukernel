// src/kernel.c

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH   80
#define VGA_HEIGHT  25

// VGA text mode の画面バッファ。
// 0xB8000 から 80x25 個の文字セルが並んでいて、1セルは 16bit。
static volatile uint16_t *const VGA_BUFFER = (volatile uint16_t *)0xB8000;

// VGA text mode の色番号。
// 背景色と文字色を 4bit ずつ組み合わせて、1byte の属性値にする。
enum vga_color {
    VGA_BLACK       = 0,
    VGA_BLUE        = 1,
    VGA_GREEN       = 2,
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

// VGA の色属性は bit 0-3 が文字色、bit 4-7 が背景色。
static inline uint8_t vga_color(enum vga_color fg, enum vga_color bg) {
    return fg | (bg << 4);
}

// VGA の1文字セルは「下位8bit: ASCII文字」「上位8bit: 色属性」。
static inline uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

static void terminal_clear(void) {
    // 画面全体を空白文字で埋める。
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(' ', term_color);
        }
    }

    term_row = 0;
    term_col = 0;
}

static void terminal_scroll(void) {
    // 2行目以降を1行ずつ上へコピーする。
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[(y - 1) * VGA_WIDTH + x] = VGA_BUFFER[y * VGA_WIDTH + x];
        }
    }

    // 最下行は新しい空行として消す。
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', term_color);
    }

    if (term_row > 0) {
        term_row--;
    }
}

static void terminal_putchar(char c) {
    if (c == '\n') {
        // 改行は次の行の先頭へ移動する。
        term_col = 0;
        term_row++;

        if (term_row >= VGA_HEIGHT) {
            terminal_scroll();
        }

        return;
    }

    if (c == '\r') {
        // キャリッジリターンは同じ行の先頭へ戻る。
        term_col = 0;
        return;
    }

    // 現在のカーソル位置へ1文字を書き込み、カーソルを進める。
    VGA_BUFFER[term_row * VGA_WIDTH + term_col] = vga_entry(c, term_color);
    term_col++;

    if (term_col >= VGA_WIDTH) {
        // 右端を越えたら自動で次の行へ折り返す。
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

static void __attribute__((unused)) panic(const char *msg) {
    // まだ割り込みやログ出力がないので、画面を赤くして停止する。
    term_color = vga_color(VGA_WHITE, VGA_RED);
    terminal_write("\nKERNEL PANIC: ");
    terminal_write(msg);

    while(1) {
        // 割り込みを止めて CPU を停止する。外部からリセットされるまで戻らない。
        __asm__ volatile ("cli; hlt");
    }
}

void kernel_main() {
    // boot.asm から呼ばれる C 側の最初の関数。
    term_color = vga_color(VGA_LIGHT_GREY, VGA_BLACK);
    terminal_clear();

    terminal_write("sakukernel booted!\n");
    terminal_write("terminal: ok\n");
    terminal_write("newline: ok\n");
    terminal_write("scroll: ok\n");

    // panic("test panic");

    while (1) {
        // カーネルが終了して戻らないように、何もない時は CPU を休ませる。
        __asm__ volatile ("hlt");
    }
}
