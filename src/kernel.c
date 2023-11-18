#include "kernel.h"

void kernel_main(){
    terminal_initialize();
    print("Hello World!\n");
    print("This is my first kernel!");

    // initialize the heap
    kheap_init();

    // initialize interrupt descriptor table
    idt_init();

    void* ptr = kmalloc(50);
    void* ptr2 = kmalloc(5000);
    void* ptr3 = kmalloc(5600);
    kfree(ptr);
    void* ptr4 = kmalloc(50);
    if(ptr || ptr2 || ptr3 || ptr4){

    }
}
