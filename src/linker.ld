ENTRY(_start)
OUTPUT_FORMAT(binary)
SECTIONS{
    . = 1M;     /* kernel will be loaded at 1mb into memory, 0x100000 is the address the kernel is loaded into */
    .text :{
        *(.text)
    }

    .rodata :{
        *(.rodata)
    }

    .data :{
        *(.data)
    }

    .bss :{
        *(COMMON)
        *(.bss)
    }
}