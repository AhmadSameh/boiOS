#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include "../terminal/terminal.h"
#include "../config.h"
#include "memory/memory.h"
#include "io/io.h"

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
void enable_interrupts(void);
void disable_interrupts(void);
extern void idt_load(struct idtr_desc* ptr);
extern void int21h(void);
extern void no_interrupt(void);
void int21h_handler(void);
void no_interrupt_handler(void);

#endif