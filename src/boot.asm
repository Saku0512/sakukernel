; src/boot.asm

BITS 32

section .multiboot
align 4
    dd 0x1BADB002 ; magic number
    dd 0x00000000 ; flags
    dd -(0x1BADB002 + 0x00000000) ; checksum

section .text
global _start
extern kernel_main

_start:
    mov esp, stack_top
    call kernel_main

.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .note.GNU-stack noalloc noexec nowrite progbits
