ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
SECTIONS{
    . = 0x400000;     /* 0x400000 is the address the programs are loaded into */
    .text : ALIGN(4096){ 
        *(.text)
    }

    .asm : ALIGN(4096){
        *(.asm)
    }

    .rodata : ALIGN(4096){
        *(.rodata)
    }

    .data : ALIGN(4096){
        *(.data)
    }

    .bss : ALIGN(4096){
        *(COMMON)
        *(.bss)
    }

}