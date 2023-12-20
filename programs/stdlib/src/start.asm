    bits    32
    global  _start
    extern  c_start
    extern  boios_exit

    section .asm

_start:
    call    c_start
    call    boios_exit
    ret