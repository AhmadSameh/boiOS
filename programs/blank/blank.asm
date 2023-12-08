    bits    32
    section .asm
    global _start

_start:
    call    get_key
    push    message 
    mov     eax, 1 
    int     0x80
    add     esp, 4 
    jmp     $

get_key:
    mov     eax, 2 ; command get key
    int     0x80
    cmp     eax, 0
    je     get_key
    ret

    section .data
message: db 'Hello, Freind!', 0