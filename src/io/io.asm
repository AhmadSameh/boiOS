    section .asm

    global  insb
    global  insw
    global  outb
    global  outw

insb:
    push    ebp
    mov     ebp, esp
    xor     eax, eax ; eax is the return value
    mov     edx, [ebp+8]
    in      al, dx  ; get byte from dx into al
    pop     ebp
    ret

insw:
    push    ebp
    mov     ebp, esp
    xor     eax, eax ; eax is the return value
    mov     edx, [ebp+8]
    in      ax, dx  ; get word from dx into ax
    pop     ebp
    ret

outb:
    push    ebp
    mov     ebp, esp
    mov     eax, [ebp+12]
    mov     edx, [ebp+8]
    out     dx, al  ; output byte from al into dx
    pop     ebp
    ret

outw:
    push    ebp
    mov     ebp, esp
    mov     eax, [ebp+12]
    mov     edx, [ebp+8]
    out     dx, ax  ; output byte from al into dx
    pop     ebp
    ret