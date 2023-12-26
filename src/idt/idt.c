#include "idt.h"
#include "kernel.h"
#include "task/task.h"
#include "task/process.h"
#include "timer/pit.h"

struct idt_desc idt_descriptors[BOIOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

static INTERRUPT_CALLBACK_FUNCTION interrupt_callbacks[BOIOS_TOTAL_INTERRUPTS];
static ISR80H_COMMAND isr80h_commands[BOIOS_MAX_ISR80H_COMMANDS];
extern void idt_load(struct idtr_desc* ptr);
extern void no_interrupt(void);
extern void isr80h_wrapper(void);
extern void* interrupt_pointer_table[BOIOS_TOTAL_INTERRUPTS];

void idt_zero(){
    outb(0x20, 0x20);
    print("Divide by zero error\n");
}

void idt_handle_exception(){
    process_terminate(task_current()->process);
    task_next();
}

void idt_clock(){
    outb(0x20, 0x20);
    // task_next();
    print("\ntestinidashfiduasghf\n");
}

void idt_init(){
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;
    for(int i=0; i<BOIOS_TOTAL_INTERRUPTS; i++)
        idt_set(i, interrupt_pointer_table[i]);
    
    idt_set(0, idt_zero);
    idt_set(0x80, isr80h_wrapper);

    for(int i=0; i<0x20; i++){
        idt_retgister_interrupt_callback(i, idt_handle_exception);
    }
    // idt_retgister_interrupt_callback(0x20, idt_zero);
    idt_retgister_interrupt_callback(0x20, pit_callback);
    idt_load(&idtr_descriptor);
    // enable_interrupts();
    
}

void idt_set(int interrupt_no, void* address){
    struct idt_desc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t) address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0;
    desc->type_attr = 0xEE;
    desc->offset_2 = (uint32_t) address >> 16;
    // idt_load(&idtr_descriptor);
}

void no_interrupt_handler(){
    outb(0x20, 0x20);
}

void interrupt_handler(int interrupt, struct interrupt_frame* interrupt_frame){
    kernel_page();
    if(interrupt_callbacks[interrupt] != 0){
        task_current_save_state(interrupt_frame);
        interrupt_callbacks[interrupt](interrupt_frame);
    }
    task_page();
    outb(0x20, 0x20);
}

int idt_retgister_interrupt_callback(int interrupt, INTERRUPT_CALLBACK_FUNCTION interrupt_callback){
    int res = BOIOS_ALL_OK;
    if(interrupt < 0 || interrupt >= BOIOS_TOTAL_INTERRUPTS){
        res = -EINVARG;
        goto out;
    }
    interrupt_callbacks[interrupt] = interrupt_callback;
out:
    return res;
}

int idt_unregister_interrupt_callback(int interrupt){
    int res = BOIOS_ALL_OK;
    if(interrupt < 0 || interrupt >= BOIOS_TOTAL_INTERRUPTS){
        res = -EINVARG;
        goto out;
    }
    interrupt_callbacks[interrupt] = 0x00;
out:
    return res;
}

void isr80h_register_command(int command_id, ISR80H_COMMAND command){
    if(command_id < 0 || command_id >= BOIOS_MAX_ISR80H_COMMANDS)
        panic("command is out of bounds\n");
    if(isr80h_commands[command_id] != 0)
        panic("you are attempting to overwrite an existing command\n");
    isr80h_commands[command_id] = command;
}

void* isr80h_handle_command(int command, struct interrupt_frame* frame){
    void* res = 0;
    if(command < 0 || command >= BOIOS_MAX_ISR80H_COMMANDS)
        return 0;
    ISR80H_COMMAND command_func = isr80h_commands[command];
    if(command_func == 0)
        return 0;
    res = command_func(frame);
    return res;
}

void* isr80h_handler(int command, struct interrupt_frame* frame){
    void* res = 0;
    kernel_page();
    task_current_save_state(frame);
    res = isr80h_handle_command(command, frame);
    task_page();
    return res;
}