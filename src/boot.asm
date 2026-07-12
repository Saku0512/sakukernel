; src/boot.asm

; Multiboot 対応ブートローダーから 32bit protected mode で呼ばれる前提。
; ここでは最低限のスタックを用意して、C の kernel_main へ処理を渡す。
BITS 32

section .multiboot
align 4
    ; Multiboot header。GRUB/QEMU が kernel.elf をカーネルとして認識するために必要。
    dd 0x1BADB002 ; magic number
    dd 0x00000000 ; flags
    dd -(0x1BADB002 + 0x00000000) ; checksum

section .text
global _start
extern kernel_main

_start:
    ; C 関数を呼ぶ前に、このカーネル専用のスタックを使うよう esp を設定する。
    mov esp, stack_top
    call kernel_main

.hang:
    ; kernel_main が万一戻ってきても、CPU を止め続ける。
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    ; 16KiB の初期スタック領域。x86 のスタックは高い番地から低い番地へ伸びる。
    resb 16384
stack_top:

; 実行可能スタック不要をリンカへ伝えるためのメタ情報。
section .note.GNU-stack noalloc noexec nowrite progbits
