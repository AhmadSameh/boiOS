#include "process.h"
#include "task.h"
#include "kernel.h"
#include "status.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "fs/file.h"
#include "string/string.h"
#include "loader/formats/elfloader.h"
#include <stdbool.h>

struct process* current_process = 0;
static struct process* processes[BOIOS_MAX_PROCESSES];

int process_load(const char* filename, struct process** process);
int process_load_for_slot(const char* filename, struct process** process, int process_slot);
// load from file
static int process_load_data(const char* filename, struct process* process);
static int process_load_binary(const char* filename, struct process* process);
static int process_load_elf(const char* filename, struct process* process);
// map to memory
int process_map_memory(struct process* process);
static int process_map_binary(struct process* process);
static int process_map_elf(struct process* process);
int process_map_stack(struct process* process);
// helper functions
static void process_init(struct process* process);
struct process* process_current();
struct process* process_get(int pid);
int process_get_free_slot();

int process_load(const char* filename, struct process** process){
    int res = 0;
    int process_slot = process_get_free_slot();
    if(process_slot < 0){
        res = -ENOMEM;
        goto out;
    }
    res = process_load_for_slot(filename, process, process_slot);
out:
    return res;
}

int process_load_for_slot(const char* filename, struct process** process, int process_slot){
    int res = 0;
    struct task* task = 0;
    struct process* _process;
    void* program_stack_ptr = 0;
    if(process_get(process_slot) != 0){
        res = -EISTKN;
        goto out;
    }
    _process = kzalloc(sizeof(struct process));
    if(!_process){
        res = -ENOMEM;
        goto out;
    }
    process_init(_process);
    res = process_load_data(filename, _process);
    if(res < 0)
        goto out;
    program_stack_ptr = kzalloc(BOIOS_USER_PROGRAM_STACK_SIZE);
    if(!program_stack_ptr){
        res = -ENOMEM;
        goto out;
    }
    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->stack = program_stack_ptr;
    _process->pid = process_slot;
    // create a task
    task = task_new(_process);
    if(ERROR_I(task) == 0){
        res = ERROR_I(task);
        goto out;
    }
    _process->task = task;
    res = process_map_memory(_process);
    if(res < 0)
        goto out;
    *process = _process;
    // add process to processess array
    processes[process_slot] = _process;
out:
    if(ISERR(res)){
        if(_process && _process->task)
            task_free(_process->task);
        // TODO: free process data
    }
    return res;
}

int process_load_switch(const char* filename, struct process** process){
    int res = process_load(filename, process);
    if(res == 0)
        process_switch(*process);
    return res;
}

int process_switch(struct process* process){
    current_process = process;
    return 0;
}

static int process_load_data(const char* filename, struct process* process){
    int res = 0;
    res = process_load_elf(filename, process);
    if(res == -EINFORMAT)
        res = process_load_binary(filename, process);
    return res;
}

static int process_load_elf(const char* filename, struct process* process){
    int res = 0;
    struct elf_file* elf_file = 0;
    res = elf_load(filename, &elf_file);
    if(ISERR(res))
        goto out;
    process->filetype = PROCESS_FILE_TYPE_ELF;
    process->elf_file = elf_file;
out:
    return res;
}

static int process_load_binary(const char* filename, struct process* process){
    int res = 0;
    int fd = fopen(filename, "r");
    if(!fd){
        res = -EIO;
        goto out;
    }
    struct file_stat stat;
    res = fstat(fd, &stat);
    if(res != BOIOS_ALL_OK)
        goto out;
    void* program_data_ptr = kzalloc(stat.file_size);
    if(!program_data_ptr){
        res = -ENOMEM;
        goto out;
    }
    if(fread(program_data_ptr, stat.file_size, 1, fd) != 1){
        res = -EIO;
        goto out;
    }
    process->filetype = PROCESS_FILE_TYPE_BIN;
    process->ptr = program_data_ptr;
    process->size = stat.file_size;
out:
    fclose(fd);
    return res;
} 

int process_map_memory(struct process* process){
    int res = 0;
    switch(process->filetype){
        case PROCESS_FILE_TYPE_ELF: res = process_map_elf(process); break;
        case PROCESS_FILE_TYPE_BIN: res = process_map_binary(process); break;
        default: panic("process_map_memory: Invalid filetype.\n");
    }
    if(res < 0)
        goto out;
    res = process_map_stack(process);
    if(res < 0)
        goto out;
out:
    return res;
}

static int process_map_elf(struct process* process){
    int res = 0;
    struct elf_file* elf_file = process->elf_file;
    struct elf32_header* header = elf_header(elf_file);
    struct elf32_phdr* phdrs = elf_pheader(header);
    for(int i=0; i<header->e_phnum; i++){
        struct elf32_phdr* phdr = &phdrs[i];
        void* phdr_phys_address = elf_phdr_phys_address(elf_file, phdr);
        int flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
        if(phdr->p_flags & PF_W)
            flags |= PAGING_IS_WRITABLE;
        res = paging_map_to(process->task->page_directory, paging_align_to_lower_page((void*)phdr->p_vaddr), paging_align_to_lower_page(phdr_phys_address), paging_align_address(phdr_phys_address+phdr->p_memsz), flags);
        if(ISERR(res))
            break;
    }   
    return res;
}

static int process_map_binary(struct process* process){
    int res = 0;
    res = paging_map_to(process->task->page_directory, (void*)BOIOS_PROGRAM_VIRTUAL_ADDRESS, process->ptr, paging_align_address(process->ptr + process->size), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE);    
    return res;
}

int process_map_stack(struct process* process){
    int res = 0;
    res = paging_map_to(process->task->page_directory, (void*)BOIOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack, paging_align_address(process->stack + BOIOS_USER_PROGRAM_STACK_SIZE), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE);
    return res;
}

static int process_find_free_allocation_index(struct process* process){
    int res = -ENOMEM;
    for(int i=0; i<BOIOS_MAX_PROGRAM_ALLOCATIONS; i++){
        if(process->allocations[i].ptr == 0){
            res = i; 
            break;
        }
    }
    return res;
}

void* process_malloc(struct process* process, size_t size){
    void* ptr = kzalloc(size);
    if(ptr == 0)
        goto out_err;
    int index = process_find_free_allocation_index(process);
    if(index < 0)
        goto out_err;
    int res = paging_map_to(process->task->page_directory, ptr, ptr, paging_align_address(ptr+size), PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if(res < 0)
        goto out_err;
    process->allocations[index].ptr = ptr;
    process->allocations[index].size = size;
    return ptr;
out_err:
    if(ptr != 0)
        kfree(ptr);
    return 0;
}

static bool process_is_process_ptr(struct process* process, void* ptr){
    for(int i=0; i<BOIOS_MAX_PROGRAM_ALLOCATIONS; i++){
        if(process->allocations[i].ptr == ptr)
            return true;
    }
    return false;
}

static void process_allocation_unjoin(struct process* process, void* ptr){
    for(int i=0; i<BOIOS_MAX_PROGRAM_ALLOCATIONS; i++){
        if(process->allocations[i].ptr == ptr){
            process->allocations[i].ptr = 0x00;
            process->allocations[i].size = 0;
        }
    }
}

static struct process_allocation* process_get_allocation_by_addr(struct process* process, void* addr){
    for(int i=0; i<BOIOS_MAX_PROGRAM_ALLOCATIONS; i++){
        if(process->allocations[i].ptr == addr)
            return &process->allocations[i];
    }
    return 0;
}

void process_free(struct process* process, void* ptr){
    // unlink pages from the process for the given address
    struct process_allocation* allocation = process_get_allocation_by_addr(process, ptr);
    // not the process's pointer
    if(!allocation)
        return;
    int res = paging_map_to(process->task->page_directory, allocation->ptr, allocation->ptr, paging_align_address(allocation->ptr+allocation->size), 0);
    if(res < 0)
        return;
    process_allocation_unjoin(process, ptr);
    kfree(ptr);
}

void process_get_arguments(struct process* process, int* argc, char***argv){
    *argc = process->arguments.argc;
    *argv = process->arguments.argv;
}

int process_count_command_arguments(struct command_argument* root_argument){
    int count = 0;
    struct command_argument* current = root_argument;
    while(current){
        current = current->next;
        count++;
    }
    return count;
}

int process_inject_arguments(struct process* process, struct command_argument* root_argument){
    int res = 0;
    struct command_argument* current = root_argument;
    int i = 0;
    int argc = process_count_command_arguments(root_argument);
    if(argc == 0){
        res = -EIO;
        goto out;
    }
    char** argv = process_malloc(process, sizeof(const char*) * argc);
    if(!argv){
        res = -ENOMEM;
        goto out;
    }
    while(current){
        char* argument_str = process_malloc(process, sizeof(current->argument));
        if(!argument_str){
            res = -ENOMEM;
            goto out;
        }
        strncpy(argument_str, current->argument, sizeof(current->argument));
        argv[i] = argument_str;
        current = current->next;
        i++;
    }
    process->arguments.argc = argc;
    process->arguments.argv = argv;
out:
    return res;
}

/*************************************************helper functions*************************************************/
static void process_init(struct process* process){
    memset(process, 0x00, sizeof(struct process));
}

struct process* process_current(){
    return current_process;
}

struct process* process_get(int pid){
    if(pid <0 || pid >= BOIOS_MAX_PROCESSES)
        return NULL;
    return processes[pid];
}

int process_get_free_slot(){
    for(int i=0; i<BOIOS_MAX_PROCESSES; i++){
        if(processes[i] == 0)
            return i;
    }
    return -EISTKN;
}
/******************************************************************************************************************/