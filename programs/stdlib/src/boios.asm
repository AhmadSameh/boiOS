    bits    32
    section     .asm
    global  print:function ; ensure elf symbol type is a function
    global  boios_getkey:function
    global  boios_malloc:function
    global  boios_free:function
    global  boios_putchar:function

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
boios_getkey:
    push    ebp
    mov     ebp, esp
    mov     eax, 2 ; command getkey
    int     0x80
    pop     ebp
    ret

; void boios_putchar(char c)
boios_putchar:
    push    ebp
    mov     ebp, esp
    mov     eax, 3 ; command putchar
    push    dword[ebp+8]
    int     0x80
    add     esp, 4
    pop     ebp
    ret

; void* boios_malloc(size_t size)
boios_malloc:
    push    ebp
    mov     ebp, esp
    mov     eax, 4 ; command malloc
    push    dword[ebp+8]
    int     0x80
    add     esp, 4
    pop     ebp
    ret

; void boios_free(void* ptr)
boios_free:
    push    ebp
    mov     ebp, esp
    mov     eax, 5
    push    dword[ebp+8]
    int     0x80
    add     esp, 4
    pop     ebp
    ret
