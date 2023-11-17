#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_desc{
    uint16_t offset_1; // offset bits 0-15
    uint16_t selector; // selector that's in our gdt, kernel code selector
    uint8_t zero;      // does nothing
    uint8_t type_attr; // descriptor type and attributes
    uint16_t offset_2; // offset bits 16-31
} __attribute__((packed));

struct idtr_desc{
    uint16_t limit; // size of descriptor table - 1
    uint32_t base;  // base address of the start of the idt
} __attribute__((packed));

void idt_init(void);
void idt_set(int interrupt_no, void* address);
extern void idt_load(struct idtr_desc* ptr);

#endif