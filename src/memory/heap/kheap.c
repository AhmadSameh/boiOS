#include "kheap.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init(){
    int total_table_entries = BOIOS_HEAP_SIZE_BYTES / BOIOS_HEAP_BLOCK_SIZE; // 100 MB / 4096 bytes will get total entries of a table
    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*)BOIOS_HEAP_TABLE_ADDRESS; // starting address of entries, which has a little over 25KB of data
    kernel_heap_table.total_entries = total_table_entries;
    void* end_of_heap = (void*)(BOIOS_HEAP_ADDRESS + BOIOS_HEAP_SIZE_BYTES); // add size of heap to its starting address to get its ending address
    int response = heap_create(&kernel_heap, (void*)BOIOS_HEAP_ADDRESS, end_of_heap, &kernel_heap_table); // create the heap of the kernel
    if(response < 0 )
        print("Failed to create heap\n");
}

void* kmalloc(size_t size){
    return heap_malloc(&kernel_heap, size);
}

void* kzalloc(size_t size){
    void* ptr = kmalloc(size);
    if(!ptr)
        return 0;
    memset(ptr, 0x00, size);
    return ptr;
}

void kfree(void* ptr){
    heap_free(&kernel_heap, ptr);
}