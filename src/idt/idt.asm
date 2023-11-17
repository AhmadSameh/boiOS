    section     .asm
    global  idt_load

idt_load:
    push    ebp ; store ebp value before starting subroutine
    mov     ebp, esp ; get argument of function in ebp
    mov     ebx, [ebp+8] ; first argument is 8 bytes after pointer
    lidt    [ebx] ; load interrupt descriptor
    pop     ebp ; get back value of ebp after subroutine 
    ret