#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include "../terminal/terminal.h"
#include "../config.h"
#include "memory/memory.h"
#include "io/io.h"

struct interrupt_frame;
typedef void*(*ISR80H_COMMAND)(struct interrupt_frame* frame);
typedef void(*INTERRUPT_CALLBACK_FUNCTION)();

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

struct interrupt_frame{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
}__attribute__((packed));

void idt_init(void);
void idt_set(int interrupt_no, void* address);
void enable_interrupts(void);
void disable_interrupts(void);
void no_interrupt_handler(void);
void isr80h_register_command(int command_id, ISR80H_COMMAND command);
int idt_retgister_interrupt_callback(int interrupt, INTERRUPT_CALLBACK_FUNCTION interrupt_callback);
int idt_unregister_interrupt_callback(int interrupt);

#endif