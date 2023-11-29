#include "kernel.h"


static struct paging_4gb_chunk* kernel_chunk = 0;
struct gdt gdt_real[BOIOS_TOTAL_GDT_SEGMENTS];
struct tss tss;
struct gdt_structured gdt_structured[BOIOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00,       .type = 0x00}, // NULL SEGMENT
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0X9A}, // kernel code segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0x92}, // kernel data segment
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xF8}, // user code segment 
    {.base = 0x00, .limit = 0xFFFFFFFF, .type = 0xF2}, // user data segment
    {.base = (uint32_t)&tss, .limit = sizeof(tss), .type = 0xE9} // tss segment, describes how segment deals with memory
};

void panic(const char* msg){
    print(msg);
    while(1);
}

void kernel_main(){
    terminal_initialize();
    print("Hello World!\n");
    print("This is my first kernel!\n");

    // initialize gdt and load it
    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, BOIOS_TOTAL_GDT_SEGMENTS);
    gdt_load(gdt_real, sizeof(gdt_real));

    // initialize the heap
    kheap_init();

    // initialize filesystems
    fs_init();

    // search and initialize disks
    disk_search_and_init();

    // initialize interrupt descriptor table
    idt_init();

    // setup and load tss
    memset(&tss, 0x00, sizeof(tss));
    tss.esp0 = 0x600000; // where kernel stack is located
    tss.ss0 = KERNEL_DATA_SELECTOR;
    tss_load(0x28); // offset in gdt

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
        fclose(fd);
        print("test\n");
    }
    while(1);
}
