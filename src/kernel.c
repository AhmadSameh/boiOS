#include "kernel.h"


static struct paging_4gb_chunk* kernel_chunk = 0;

void kernel_main(){
    terminal_initialize();
    print("Hello World!\n");
    print("This is my first kernel!\n");

    // initialize the heap
    kheap_init();

    // initialize filesystems
    fs_init();

    // search and initialize disks
    disk_search_and_init();

    // initialize interrupt descriptor table
    idt_init();

    // setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    // switch to kernel paging chunk
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));
    // enable paging
    enable_paging();

    // enable interrupts
    enable_interrupts();

    int fd = fopen("0:/hello.txt", "r");
    if(fd){
        // print("we opened hello.txt!\n");
        // char buf[23];
        // fseek(fd, 2, SEEK_SET);
        // fread(buf, 20, 1, fd);
        // buf[22] = 00;
        // print(buf);
        struct file_stat s;
        fstat(fd, &s);
    }
    while(1);
}
