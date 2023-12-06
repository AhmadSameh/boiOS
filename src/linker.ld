ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS{
    . = 1M;     /* kernel will be loaded at 1mb into memory, 0x100000 is the address the kernel is loaded into */
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