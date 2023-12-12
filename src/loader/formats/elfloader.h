#ifndef ELFLOADER_H
#define ELFLOADER_H

#include <stdint.h>
#include <stddef.h>
#include "elf.h"
#include "../../config.h"

struct elf_file{
    char filename[BOIOS_MAX_PATH];
    int in_memory_size;
    // physical address where file is laoded at
    void* elf_memory;
    // virtual base address of this binary
    void* virtual_base_address;
    // ending virtual address of this binary
    void* virtual_end_address;
    // physical base address of this binary
    void* physical_base_address;
    // physical end address of this binary 
    void* physical_end_address;
};

int elf_load(const char* filename, struct elf_file** file_out);
void elf_close(struct elf_file* elf_file);

#endif 