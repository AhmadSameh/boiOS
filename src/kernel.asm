        bits 32

        global  _start
        extern  kernel_main

        CODE_SEG        equ 0x08
        DATA_SEG        equ 0x10

_start: 
        ; put the data segment into all the other segments, gdt
        mov     ax, DATA_SEG
        mov     ds, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax
        mov     ss, ax
        mov     ebp, 0x00200000
        mov     esp, ebp

        ; enable a20 line which allows access to the 21st bit of any memory access
        in      al, 0x92
        or      al, 2
        out     0x92, al

        ; remap master PIC
        mov     al, 00010001b
        out     0x20, al ; tell master pic

        mov     al, 0x20 ; int 0x20 is where master ISR should start
        out     0x21, al

        mov     al, 00000001b
        out     0x21, al
        ; end remap master PIC

        ; enable interrupts, processor has to enable interrupts so that they are not ignored 
        sti 

        call kernel_main

        jmp     $

times   512-($ - $$) db 0 
