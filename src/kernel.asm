[BITS 32]

        global _start

        CODE_SEG    equ 0x08
        DATA_SEG    equ 0x10

_start: 
        ; put the data segment into all the other segments
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

        jmp     $