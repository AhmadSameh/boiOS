; the boot loader is what loads the kernel into memory

        org    0x7c00
        bits    16       ; use 16 bit architecture, because during the boot process, real mode is on which only works on 16 bits

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start 

;--------------------;bios parameter block;-------------------;
_start: ; first 3 bytes of the bios parameter block
        jmp short start
        nop
times 33 db 0 ; create 33 bytes after the short jump
;-------------------------------------------------------------;

start:
        jmp     0:step2 ; make code segment to 0x7c0

step2:
        cli ; clear interrupt flags, to disable interrupts to change segment registers so hardware interrupts do not interfere
        ; change data segments
        mov     ax, 0
        mov     ds, ax    ; must copy the segment first to ax then ds, no direct addressing on ds
        mov     es, ax    ; gotta change these data segments and not count on bios
        ; change stack segment
        mov     ss, ax    
        mov     sp, 0x7c00 ; set stack pointer to 0x7c00
        sti ; enable interrupts 

.load_protected:
        cli
        lgdt[gdt_descriptor]
        mov     eax, cr0
        or      eax, 0x01
        mov     cr0, eax
        jmp     CODE_SEG:load32

;GDT
gdt_start:

gdt_null:
        dd      0x00
        dd      0x00

;offset 0x8
gdt_code:       ; CS should point to this
        dw      0xffff  ; segment limit first 0-15 bits
        dw      0       ; base first 0-15 bits
        db      0       ; base 16-23 bits
        db      0x9a    ; access byte, a bunch of bitmasks
        db      11001111b ; high 4 bit flags and 4 low bit flags
        db      0
; offset 0x10 
gdt_data:        ; DS, ES, SS, FS, GS should point to this
        dw      0xffff  ; segment limit first 0-15 bits
        dw      0       ; base first 0-15 bits
        db      0       ; base 16-23 bits
        db      0x92    ; access byte, a bunch of bitmasks
        db      11001111b ; high 4 bit flags and 4 low bit flags
        db      0
gdt_end:

gdt_descriptor:
        dw      gdt_end - gdt_start - 1
        dd      gdt_start

[BITS 32]
load32:
        mov     ax, DATA_SEG
        mov     ds, ax
        mov     es, ax
        mov     fs, ax
        mov     gs, ax
        mov     ss, ax
        mov     ebp, 0x00200000
        mov     esp, ebp
        jmp     $

times   510-($ - $$) db 0 ; make sure the code is at least 510 bytes by padding 0s to the end of the written code
dw      0xaa55            ; sp that the boot signal is at bytes 511 and 513

