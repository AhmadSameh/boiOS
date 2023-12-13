    bits 32
    global print:function ; ensure elf symbol type is a function
    global getkey:function

; void print(const char* msg)
print:
    push    ebp
    mov     ebp, esp
    push    dword[ebp+8]
    mov     eax, 1 ; print command
    int     0x80
    add     esp, 4
    pop     ebp
    ret

; int getkey()
getkey:
    push    ebp
    mov     ebp, esp
    mov     eax, 2 ; command getkey
    int     0x80
    pop     ebp
    ret