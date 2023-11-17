#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "terminal/terminal.h"
#include "idt/idt.h"
#include "io/io.h"

void kernel_main(){
    terminal_initialize();
    print("Hello World!\n");
    print("This is my first kernel!");

    // initialize interrupt descriptor table
    idt_init();

    outb(0x60, 0xff);
}
