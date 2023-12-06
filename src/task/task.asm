    bits    32
    section .asm
    global  restore_general_purpose_registers
    global  user_mode_enter
    global  user_registers
    
user_mode_enter:
    mov     ebp, esp
    ; push data segment (ss)
    ; push stack address
    ; push flags
    ; push code segment
    ; push ip
    ; access structure passed
    mov     ebx, [ebp+4]
    ; push data/stack selector
    push    dword [ebx+44]
    ; push stack pointer
    push    dword [ebx+40]
    ; push flags, oring interrupt bits
    pushf
    pop     eax
    or      eax, 0x200
    push    eax
    ; push code segment
    push    dword [ebx+32]
    ; push ip 
    push    dword [ebx+28]
    ; setup some segment registers
    mov     ax, [ebx+44]
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    push    dword [ebp+4]
    call    restore_general_purpose_registers
    add     esp, 4
    ; leave kernel land and execute in user land,
    ; falsifying an interrupt stack frame 
    iretd

; void restore_general_purpose_registers(struct registers* regs);
restore_general_purpose_registers:
    push    ebp
    mov     ebp, esp
    mov     ebx, [ebp+8]
    mov     edi, [ebx]
    mov     esi, [ebx+4]
    mov     ebp, [ebx+8]
    mov     edx, [ebx+16]
    mov     ecx, [ebx+20]
    mov     eax, [ebx+24]
    mov     ebx, [ebx+12]
    pop     ebp
    ret

; change segment registers to user data segment registers
user_registers:
    mov     ax, 0x23
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    ret
