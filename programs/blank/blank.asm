    bits    32
    section .asm
    global _start

_start:

_loop:
    call    get_key
    push    eax
    mov     eax, 3  
    int     0x80
    add     esp, 4 
    jmp     _loop

get_key:
    mov     eax, 2 ; command get key
    int     0x80
    cmp     eax, 0
    je     get_key
    ret

    section .data
message: db 'Hello, Freind!', 0