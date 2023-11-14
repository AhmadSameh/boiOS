; the boot loader is what loads the kernel into memory

        org     0       
        bits    16    ; use 16 bit architecture, because during the boot process, real mode is on which only works on 16 bits


;--------------------;bios parameter block;-------------------;
_start: ; first 3 bytes of the bios parameter block
        jmp short start
        nop
times 33 db 0 ; create 33 bytes after the short jump
;-------------------------------------------------------------;

start:
        jmp     0x7c0:step2 ; make code segment to 0x7c0


step2:
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

;--------------------;read from disk the old way, cylindrical;-------------------;
; data is read from the disk into the extra segment, with BX the offset
        mov     ah, 2 ; read sector command
        mov     al, 1 ; read 1 sector
        mov     ch, 0 ; cylinder number = 0
        mov     cl, 2 ; sector number = 2
        mov     dh, 0 ; head number = 0
        mov     bx, buffer
        int     0x13  ; invoke read command
        jc      error
;--------------------------------------------------------------------------------;

        mov     si, buffer
        call    print 
        
        jmp     $

error:
        mov     si, error_msg
        call    print
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

error_msg: db   'Failed to load sector!'

times   510-($ - $$) db 0 ; make sure the code is at least 510 bytes by padding 0s to the end of the written code
dw      0xaa55            ; sp that the boot signal is at bytes 511 and 513

buffer: ; this is to write to it from disk
        ; even though it is beyond the bootlader space, it can still be refrenced but not accessed by the bootloader
