#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "task.h"
#include "../config.h"


struct process{
    uint16_t pid;
    char filename[BOIOS_MAX_PATH];
    // main process task
    struct task* task;
    // keep track of allocations memory so it can be freed later
    void* allocations[BOIOS_MAX_PROGRAM_ALLOCATIONS];
    // physical pointer to process memory
    void* ptr;
    // phyiscal pointer to stack memory
    void* stack;
    // size of data pointed to by ptr
    uint32_t size;

    struct keyboard_buffer{
        char buffer[BOIOS_KEYBOARD_BUFFER_SIZE];
        int tail;
        int head; 
    } keyboard;
};

int process_load_for_slot(const char* filename, struct process** process, int process_slot);
int process_load(const char* filename, struct process** process);
struct process* process_current(void);
struct process* process_get(int pid);

#endif