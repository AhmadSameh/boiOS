#include "heap.h"

int heap_create(struct heap* heap, void* ptr, void* end_of_heap, struct heap_table* table){
    int response = 0;
    // make sure that starting and ending addresses are aligned to 4096 byte blocks
    if(!heap_validate_alignment(ptr) || !heap_validate_alignment(end_of_heap)){
        response = -EINVARG;
        goto out;
    }
    // create the heap structure in memory
    memset(heap, 0, sizeof(struct heap));
    heap->start_address = ptr;
    heap->table = table;
    // validate starting and ending addresses, that they match the table total entries
    response = heap_validate_table(ptr, end_of_heap, table);
    if(response < 0)
        goto out;
    // create the table entries, marking each entry to be free
    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total_entries;
    memset(table->entries, HEAP_BLOCK_TABLE_ENTRY_FREE, table_size);
out: 
    return response;
}

void* heap_malloc(struct heap* heap, size_t size){
    size_t aligned_size = (size_t)heap_align_value_to_upper(size); // make sure size is aligned to the block size
    uint32_t total_blocks = aligned_size / BOIOS_HEAP_BLOCK_SIZE; //get total number of blocks total size / block size
    return heap_malloc_blocks(heap, total_blocks); // allocate calculated blocks
}

void* heap_malloc_blocks(struct heap* heap, uint32_t total_blocks){
    void* address = 0;
    int start_block = heap_get_start_block(heap, total_blocks); // find the first block which has following blocks that fit the size
    if(start_block < 0) // if start block not found then there is no memory
        goto out;
    // translate the start block number to the returning address
    address = heap_block_to_address(heap, start_block);
    // mark blocks as taken
    heap_mark_blocks_taken(heap, start_block, total_blocks);
out:
    return address;
}

int heap_get_start_block(struct heap* heap, uint32_t total_blocks){
    struct heap_table* table = heap->table;
    int current_block = 0; 
    int block_start = -1; 
    for(size_t i=0; i<table->total_entries; i++){
        // if entry is not free, reset state
        if(heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_TABLE_ENTRY_FREE){
            current_block = 0;
            block_start = -1;
            continue;
        }
        // first block is found
        if(block_start == -1)
            block_start = i;
        current_block++; // number of blocks increases until required size is satisfied
        // we now have starting block following enough empty blocks to allocate
        if(current_block == total_blocks)
            break;
    }
    //if block start was still 1 then no memory was found to allocate
    if(block_start == -1)
        return -ENOMEM;
    return block_start;
}

void* heap_block_to_address(struct heap* heap, int start_block){
    return heap->start_address + (start_block * BOIOS_HEAP_BLOCK_SIZE); // translate block number to address by adding offset to starting address
}

void heap_mark_blocks_taken(struct heap* heap, int start_block, int total_blocks){
    int end_block = (start_block + total_blocks) - 1; // get address of last block
    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST; // mark the first block and that it is taken
    // if there are more blocks start the next chain
    if(total_blocks > 1)
        entry |= HEAP_BLOCK_HAS_NEXT;
    for(int i=start_block; i<=end_block; i++){
        heap->table->entries[i] = entry;
        // reset block to entry taken for the next loop
        entry = HEAP_BLOCK_TABLE_ENTRY_TAKEN;
        // keep setting the has next flag until the last block
        if(i != end_block - 1)
            entry |= HEAP_BLOCK_HAS_NEXT;
    }
}

void heap_free(struct heap* heap, void* ptr){
    heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}

int heap_address_to_block(struct heap* heap, void* address){
    return ((int)(address - heap->start_address)) / BOIOS_HEAP_BLOCK_SIZE; // get block number from address
}

void heap_mark_blocks_free(struct heap* heap, int starting_block){
    struct heap_table* table = heap->table;
    for(int i=starting_block; i<(int)table->total_entries; i++){
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        // free the table etnry
        table->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
        // keep freeing until the last block, no longer has next flag
        if(!(entry & HEAP_BLOCK_HAS_NEXT))
            break;
    }
}

static uint32_t heap_align_value_to_upper(uint32_t val){
    if(val % BOIOS_HEAP_BLOCK_SIZE == 0)
        return val;
    // add bits to align to block size
    val = (val - (val % BOIOS_HEAP_BLOCK_SIZE));
    val += BOIOS_HEAP_BLOCK_SIZE;
    return val;
}

static bool heap_validate_alignment(void* ptr){
    return ((unsigned int)ptr % BOIOS_HEAP_BLOCK_SIZE) == 0; // validate address aligns with block size 
}

// validate if the table knows how many blocks are need between ptr and end_of_heap, ie. size of table
static int heap_validate_table(void* ptr, void* end_of_heap, struct heap_table* table){
    int response = 0;
    size_t table_size = (size_t)(end_of_heap - ptr);
    size_t total_blocks = table_size / BOIOS_HEAP_BLOCK_SIZE;
    if(table->total_entries != total_blocks){
        response = -EINVARG;
        goto out;
    }
out:
    return response;
}

static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry){
    return entry & 0x0F;
}