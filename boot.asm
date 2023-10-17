; the boot loader is what loads the kernel into memory

        org     0       
        bits    16    ; use 16 bit architecture, because during the boot process, real mode is on which only works on 16 bits
        jmp     0x7c0:start ; make code segment to 0x7c0
start:
        cli ; clear interrupt flags, to disable interrupts to change segment registers so hardware interrupts do not interfere
        ; change data segments
        mov     ax, 0x7c0
        mov     ds, ax    ; must copy the segment first to ax then ds, no direct addressing on ds
        mov     es, ax    ; gotta change these data segments and not count on bios
        ; change stack segment
        mov     ax, 0x00
        mov     ss, ax    
        mov     sp, 0x7c00 ; set stack pointer to 0x7c00
        sti ; enable interrupts 
        mov     si, message ; a pointer to the start of the message we want to print
        call    print  ; print the message 
        jmp     $  ; jump to the same line

print:  ; function to print a string
        mov     bx, 0  ; bh for page number and bl for foreground color
.loop:
        lodsb   ; load character the si register is pointing to to the al register
        cmp     al, 0       ; if al = 0, the string reached the null character
        je      .done       ; then jump to the done segment
        call    print_char  ; else print the character in al
        jmp     .loop    ; loop back to print the other characters
.done:
        ret

print_char:  ; function to print a singular character
        mov     ah, 0eh   ; bios routine
        int     0x10      ; to print a character
        ret

message: db 'Hello World!', 0 ; define the message we want to print
times   510-($ - $$) db 0 ; make sure the code is at least 510 bytes by padding 0s to the end of the written code
dw      0xaa55            ; sp that the boot signal is at bytes 511 and 513