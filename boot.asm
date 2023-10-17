; the boot loader is what loads the kernel into memory

        org     0x7c00   ; the adress where the bios loads the bootloader
        bits    16    ; use 16 bit architecture, because during the boot process, real mode is on which only works on 16 bits

start:
        mov     si, message ; a pointer to the start of the message we want to print
        call    print  ; print the message 
        jmp    $  ; jump to the same line

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