#include "paging.h"

static uint32_t* current_directory = 0;

void paging_load_directory(uint32_t* directory);

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

void paging_switch(uint32_t* directory){
    paging_load_directory(directory); 
    current_directory = directory;
}