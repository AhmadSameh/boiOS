#include "paging.h"
#include "../../status.h"

static uint32_t* current_directory = 0;

void paging_load_directory(uint32_t* directory);
int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out);
bool paging_is_aligned(void* address);

// function gives a linear memory space, so virtual memory address is the same as the physical memory
struct paging_4gb_chunk* paging_new_4gb(uint8_t flags){
    // set all table entries to 0, 32 bits times 1024 entry 4096 bytes
    uint32_t* directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    // offset is used to add more table entries to the directory with entry of size 4mb
    int offset = 0;
    // create a table for each entry in directory 
    for(int i=0; i<PAGING_TOTAL_ENTRIES_PER_TABLE; i++){
        // each table entry in the directory is 4096 bytes, create entry with all bits set to 0
        uint32_t* entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        // each entry is a 4096 byte chunk, initialized with the flags
        for(int j=0; j<PAGING_TOTAL_ENTRIES_PER_TABLE; j++)
            entry[j] = (offset + j * PAGING_PAGE_SIZE) | flags;
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        // make entry writable by default
        directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITABLE; 
    }
    // return the 4gb chunk with the directory
    struct paging_4gb_chunk* chunk_4gb = kzalloc(sizeof(struct paging_4gb_chunk));
    chunk_4gb->directory_entry = directory;
    return chunk_4gb;
}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk){
    return chunk->directory_entry;
}

void paging_switch(struct paging_4gb_chunk* directory){
    paging_load_directory(directory->directory_entry); 
    current_directory = directory->directory_entry;
}

int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out){
    int response = 0;
    // ensure if virtual address is aligned and a valid paging address
    if(!paging_is_aligned(virtual_address)){
        response = -EINVARG;
        goto out;
    }
    // calculate directory entry index and table entry index in the paging directory from the given virtual address
    *directory_index_out = ((uint32_t)virtual_address) / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
    *table_index_out = ((uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE); 
out:
    return response;
}

int paging_set(uint32_t* directory, void* virtual_address, uint32_t value_of_entry){
    // ensure if virtual address is aligned and a valid paging address
    if(!paging_is_aligned(virtual_address))
        return -EINVARG;
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    // get directory and table indexes from virtual address
    int response = paging_get_indexes(virtual_address, &directory_index, &table_index);
    if(response < 0)
        return response;
    // etnry is taken from the directory array using the calculated directory index and so is the table from upper 20 bits of entry
    uint32_t entry = directory[directory_index];
    uint32_t* table = (uint32_t*)(entry & 0xFFFFF000);
    // set the table index to the given entry value
    table[table_index] = value_of_entry;
    return 0;
}

bool paging_is_aligned(void* address){
    return ((uint32_t)address % PAGING_PAGE_SIZE) == 0;
}

void paging_free_4gb(struct paging_4gb_chunk* chunk){
    for(int i=0; i<PAGING_TOTAL_ENTRIES_PER_TABLE; i++){
        uint32_t entry = chunk->directory_entry[i];
        uint32_t* table = (uint32_t*)(entry & 0xFFFFF000); // clear flags to get actual page table address
        kfree(table);
    }
    kfree(chunk->directory_entry);
    kfree(chunk);
}

void* paging_align_address(void* ptr){
    if((uint32_t)ptr % PAGING_PAGE_SIZE)
        return (void*)((uint32_t)ptr + PAGING_PAGE_SIZE - ((uint32_t)ptr % PAGING_PAGE_SIZE));
    return ptr;
}

void* paging_align_to_lower_page(void* addr){
    uint32_t _addr = (uint32_t)addr;
    _addr -= _addr % PAGING_PAGE_SIZE;
    return (void*)_addr;
}

int paging_map(struct paging_4gb_chunk* directory, void* virtual_address, void* physical_address, int flags){
    if(((unsigned int)virtual_address % PAGING_PAGE_SIZE) || ((unsigned int)physical_address % PAGING_PAGE_SIZE))
        return -EINVARG;
    return paging_set(directory->directory_entry, virtual_address, (uint32_t)physical_address | flags);
}

int paging_map_range(struct paging_4gb_chunk* directory, void* virtual_address, void* physical_address, int total_pages, int flags){
    int res = 0;
    for(int i=0; i<total_pages; i++){
        res = paging_map(directory, virtual_address, physical_address, flags);
        if(res < 0)
            break;
        virtual_address += PAGING_PAGE_SIZE;
        physical_address += PAGING_PAGE_SIZE;
    }
    return res;
}

int paging_map_to(struct paging_4gb_chunk* directory, void* virtual_address, void* physical_address, void* physical_end_address, int flags){
    int res = 0;
    if((uint32_t)virtual_address % PAGING_PAGE_SIZE || (uint32_t)physical_address % PAGING_PAGE_SIZE || (uint32_t)physical_end_address % PAGING_PAGE_SIZE){
        res = -EINVARG;
        goto out;
    }
    if((uint32_t)physical_end_address < (uint32_t)physical_address){
        res = -EINVARG;
        goto out;
    }
    uint32_t total_bytes = physical_end_address - physical_address;
    int total_pages = total_bytes / PAGING_PAGE_SIZE;
    res = paging_map_range(directory, virtual_address, physical_address, total_pages, flags);
out:
    return res;
}

uint32_t paging_get(uint32_t* directory, void* virtual_address){
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    paging_get_indexes(virtual_address, &directory_index, &table_index);
    uint32_t entry = directory[directory_index];
    uint32_t* table = (uint32_t*)(entry & 0xFFFFF000);
    return table[table_index];
}