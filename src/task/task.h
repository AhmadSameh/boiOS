#ifndef TASK_H
#define TASK_H

#include "../kernel.h"
#include "../status.h"
#include "../config.h"
#include "../memory/paging/paging.h"
#include "../memory/heap/kheap.h"

struct interrupt_frame;

struct registers{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip; // instruction pointer
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct process;

struct task{
    struct paging_4gb_chunk* page_directory; // page directory of task
    struct registers registers; // registers of task, when task is not running
    struct task* next; // next task
    struct task* prev; // previous task
    struct process* process; // process that task is running
};

struct task* task_new(struct process* process);
struct task* task_current(void);
struct task* task_get_next(void);
int task_free(struct task *task);
int task_switch(struct task* task);
int task_page(void);
int task_page_task(struct task* task);
void task_next(void);
void task_run_first_ever_task(void);
void user_mode_enter(struct registers *regs);
void restore_general_purpose_registers(struct registers* regs);
void user_registers(void);
void task_current_save_state(struct interrupt_frame* frame);
int copy_string_from_task(struct task* task, void* virual_address, void* physical_address, int max);
void* task_get_stack_item(struct task* task, int index);
void* task_virtual_address_to_physical(struct task* task, void* virt_addr);

#endif