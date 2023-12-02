#include "task.h"
#include "process.h"

struct task* current_task = 0;
struct task* task_tail = 0;
struct task* task_head = 0;

void task_run_first_ever_task(){
    if(!current_task)
        panic("task_run_first_ever_task(): no current task exists!\n");
    task_switch(task_head);
    task_return(&task_head->registers);
}

struct task* task_current(){
    return current_task;
}

int task_switch(struct task* task){
    current_task = task;
    paging_switch(task->page_directory->directory_entry);
    return 0;
}

// takes us out of the kernel page directory and load us into the task page directory
int task_page(){
    user_registers();
    task_switch(current_task);
    return 0;
}

int task_init(struct task* task, struct process* process){
    memset(task, 0, sizeof(struct task));
    // map entire 4gb address space to itself by creating a page directory for the task, read only address space
    task->page_directory = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if(!task->page_directory)
        return -EIO;
    task->registers.ip = BOIOS_PROGRAM_VIRTUAL_ADDRESS;
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.esp = BOISOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START;
    task->process = process;
    return 0;
}

struct task* task_get_next(){
    if(!current_task->next)
        return task_head;
    return current_task->next;
}

static void task_list_remove(struct task* task){
    if(task->prev)
        task->prev->next = task->next;
    if(task == task_head)
        task_head = task->next;
    if(task == task_tail)
        task_tail = task->prev;
    if(task == current_task)
        current_task = task_get_next();
}

int task_free(struct task *task){
    paging_free_4gb(task->page_directory);
    task_list_remove(task);
    kfree(task);
    return 0;
}

// call this when in need to create a new task
struct task* task_new(struct process* process){
    int response = 0;
    struct task* task = kzalloc(sizeof(struct task));
    if(!task){
        response = -ENOMEM;
        goto out;
    }
    response = task_init(task, process);
    if(response != BOIOS_ALL_OK)
        goto out;
    if(task_head == 0){
        task_head = task;
        task_tail = task;
        goto out;
    }
    // add task to the tail of the list
    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;
out:
    if(ISERR(response)){
        task_free(task);
        return ERROR(response);
    }
    return task;
}