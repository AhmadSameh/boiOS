; the boot loader is what loads the kernel into memory

        org    0x7c00
        bits    16       ; use 16 bit architecture, because during the boot process, real mode is on which only works on 16 bits

        CODE_SEG equ gdt_code - gdt_start
        DATA_SEG equ gdt_data - gdt_start 

;--------------------;bios parameter block;-------------------;
; first 3 bytes of the bios parameter block
        jmp short start
        nop
; FAT16 header
        OEMIdentifier           db 'BOIOS   '
        BytesPerSector          dw 0x200 ; 512 bytes per sector, ignored by most kernels
        SectorsPerCluster       db 0x80 
        ReservedSectors         dw 200 ; space where entire kernel will be stored
        FATCopies               db 0x02
        RootDirEntries          dw 0x40 
        NumSectors              dw 0x00
        MediaType               db 0xf8
        SectorsPerFat           dw 0x100
        SectorPerTrack          dw 0x20
        NumberOfHeads           dw 0x40
        HiddenSectors           dd 0x00
        SectorsBig              dd 0x773594
; extended bpb (Dos 4.0)
        DriveNumber             db 0x80
        WinNTBit                db 0x00
        Signature               db 0x29
        VolumeID                dd 0xd105
        VolumeIDString          db 'BOIOS BOOT '
        SystemIDString          db 'FAT16   '
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
        lgdt[gdt_descriptor] ; load lgdt registers with addres of gdt_descriptor
        mov     eax, cr0
        or      eax, 0x01
        mov     cr0, eax
        jmp     CODE_SEG:load32 ; move the cs to the code segment

;--------------------;global descriptor table;-------------------;
gdt_start:
; first entry is always null
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
;----------------------------------------------------------------;

;--------------------;load the kernel into memory;-------------------;
        bits    32
load32:
        mov     eax, 1 ; starting sector we want to load from
        mov     ecx, 100 ; the total number of sectors we want to load
        mov     edi, 0x0100000 ; address we want to load them into
        call    ata_lba_read
        jmp     CODE_SEG:0x0100000

; dummy disk driver to get kernel loaded and load sectors into memory
ata_lba_read:
        mov     ebx, eax ; backup LBA
        ; send highest 8 bits of lba to hard disk controller
        shr     eax, 24
        or      eax, 0xe0 ; select master drive
        mov     dx, 0x1f6 ; port to write 8 bits to
        out     dx, al ; finished sending 8 bits to the lba
        ; send total sectors to read
        mov     eax, ecx 
        mov     dx, 0x1f2
        out     dx, al ; finished sending
        ; send more bits of lba
        mov     eax, ebx ; restore backup lba
        mov     dx, 0x1f3
        out     dx, al ; finished sending
        ; send more bits of lba
        mov     dx, 0x1f4
        mov     eax, ebx ; restore backup lba
        shr     eax, 8
        out     dx, al ; finished sending      
        ; send upper 16 bits of lba
        mov     dx, 0x1f5
        mov     eax, ebx
        shr     eax, 16
        out     dx, al

        mov     dx, 0x1f7
        mov     al, 0x20
        out     dx, al

; read all sectors into memory
.next_sector:
        push    ecx

; check if we need to read
.try_again:
        mov     dx, 0x1f7
        in      al, dx
        test    al, 8
        jz      .try_again
        ; read 256 words at a time
        mov     ecx, 256
        mov     dx, 0x1f0
        rep     insw  ; does insw instruction 256 times, stored in ecx
        pop     ecx
        loop    .next_sector ; decrement ecx and jump to .next_sector
        ; end of reading sectors
        ret
;--------------------------------------------------------------------;

times   510-($ - $$) db 0 ; make sure the code is at least 510 bytes by padding 0s to the end of the written code
dw      0xaa55            ; sp that the boot signal is at bytes 511 and 513

