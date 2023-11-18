#include "idt.h"

struct idt_desc idt_descriptors[BOIOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

void idt_zero(){
    print("Divide by zero error\n");
}

void idt_init(){
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;

    for(int i=0; i<BOIOS_TOTAL_INTERRUPTS; i++)
        idt_set(i, no_interrupt);
    idt_set(0, idt_zero);
    idt_set(0x21, int21h);
}

void idt_set(int interrupt_no, void* address){
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t) address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0;
    desc->type_attr = 0xEE;
    desc->offset_2 = (uint32_t) address >> 16;
    idt_load(&idtr_descriptor);
}

void int21h_handler(){
    print("Keyboard pressed!\n");
    outb(0x20, 0x20);
}

void no_interrupt_handler(){
    outb(0x20, 0x20);
}