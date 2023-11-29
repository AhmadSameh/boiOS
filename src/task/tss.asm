    section .asm
    global  tss_load

tss_load:
    push    ebp
    mov     ebp, esp
    mov     eax, [esp+8] ; tss segment
    ltr     ax
    pop     ebp
    ret