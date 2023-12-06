#include "process.h"
#include "task.h"
#include "../kernel.h"
#include "../status.h"
#include "../memory/memory.h"
#include "../memory/heap/kheap.h"
#include "../memory/paging/paging.h"
#include "../fs/file.h"
#include "../string/string.h"

struct process* current_process = 0;
static struct process* processes[BOIOS_MAX_PROCESSES];

int process_load(const char* filename, struct process** process);
int process_load_for_slot(const char* filename, struct process** process, int process_slot);
// load from file
static int process_load_data(const char* filename, struct process* process);
static int process_load_binary(const char* filename, struct process* process);
// map to memory
int process_map_memory(struct process* process);
int process_map_binary(struct process* process);
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

static int process_load_data(const char* filename, struct process* process){
    int res = 0;
    res = process_load_binary(filename, process);
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
    process->ptr = program_data_ptr;
    process->size = stat.file_size;
out:
    fclose(fd);
    return res;
} 

int process_map_memory(struct process* process){
    int res = 0;
    res = process_map_binary(process);
    if(res < 0)
        goto out;
    // map the stack, giving the task access to write to it
    paging_map_to(process->task->page_directory, (void*)BOIOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack, paging_align_address(process->stack + BOIOS_USER_PROGRAM_STACK_SIZE), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE);
out:
    return res;
}

int process_map_binary(struct process* process){
    int res = 0;
    paging_map_to(process->task->page_directory, (void*)BOIOS_PROGRAM_VIRTUAL_ADDRESS, process->ptr, paging_align_address(process->ptr + process->size), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE);    
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