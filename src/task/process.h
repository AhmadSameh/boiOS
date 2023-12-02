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
};

int process_load_for_slot(const char* filename, struct process** process, int process_slot);
int process_load(const char* filename, struct process** process);

#endif