#include "kernel.h"

static struct paging_4gb_chunk* kernel_chunk = 0;

void kernel_main(){
    terminal_initialize();
    print("Hello World!\n");
    print("This is my first kernel!");

    // initialize the heap
    kheap_init();

    // initialize interrupt descriptor table
    idt_init();

    // setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    // switch to kernel paging chunk
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));
    // enable paging
    enable_paging();

    char buf[512];
    disc_read_sector(0, 1, buf);

    // enable interrupts
    enable_interrupts();
}
