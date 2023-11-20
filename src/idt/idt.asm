    section     .asm
    
    global  idt_load
    global  int21h
    global  no_interrupt
    global enable_interrupts
    global disable_interrupts

    extern  int21h_handler
    extern  no_interrupt_handler

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

idt_load:
    push    ebp ; store ebp value before starting subroutine
    mov     ebp, esp ; get argument of function in ebp
    mov     ebx, [ebp+8] ; first argument is 8 bytes after pointer
    lidt    [ebx] ; load interrupt descriptor
    pop     ebp ; get back value of ebp after subroutine 
    ret

int21h:
    cli
    pushad
    call    int21h_handler
    popad
    sti
    iret

no_interrupt:
    cli
    pushad
    call    no_interrupt_handler
    popad
    sti
    iret