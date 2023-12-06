    section     .asm
    
    global  idt_load
    global  int21h
    global  no_interrupt
    global  enable_interrupts
    global  disable_interrupts
    global  isr80h_wrapper

    extern  int21h_handler
    extern  isr80h_handler
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
    pushad
    call    int21h_handler
    popad
    sti
    iret

no_interrupt:
    pushad
    call    no_interrupt_handler
    popad
    sti
    iret

isr80h_wrapper:
    ; interrupt frame start
    ; the following is already pushed to us by the processor upon entry from interrupt
    ; uint32_t ip;
    ; uint32_t cs;
    ; uint32_t flags;
    ; uint32_t sp;
    ; uint32_t ss;
    ; push general purpose registers to stack
    pushad
    ; interrupt frame end
    ; push sp so we point to interrupt frame, can be accessed as a struct from a c code
    push    esp
    ; contains command the kernel should invoke, push it to stack for isr80h_handler
    push    eax
    call    isr80h_handler
    mov     dword[tmp_res], eax
    add     sp, 8
    ; restore general purpose registers for user
    popad
    mov     eax, [tmp_res]
    iretd


    section .data
; here is the return result from isr80h_handler
tmp_res: dd 0