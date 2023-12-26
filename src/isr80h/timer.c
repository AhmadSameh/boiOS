#include "timer.h"
#include "timer/pit.h"
#include "idt/idt.h"

void* isr80h_command11_sleep(struct interrupt_frame* frame){
    int millis = (int)task_get_stack_item(task_current(), 0);
    enable_interrupts();
    sleep(millis);
    disable_interrupts();
    return 0;
}