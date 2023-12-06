    bits    32
    section .asm
    global _start

_start:
    push    20
    push    30
    mov     eax, 0 ; command 0 -> sum
    int     0x80
    add     esp, 8 ; restore stack by removing the 2 pushed arguments
    jmp     $