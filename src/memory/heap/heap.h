#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "memory/memory.h"
#include "status.h"
#include "config.h"

#define HEAP_BLOCK_TABLE_ENTRY_FREE     0x00
#define HEAP_BLOCK_TABLE_ENTRY_TAKEN    0x01

#define HEAP_BLOCK_HAS_NEXT     0b10000000
#define HEAP_BLOCK_IS_FIRST     0b01000000     

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

struct heap_table{
    HEAP_BLOCK_TABLE_ENTRY* entries; // start address of the heap table entries, each entry is 8 bits
    size_t total_entries;
};

struct heap{
    struct heap_table* table;
    void* start_address; // start address of heap data pool
};

int heap_create(struct heap* heap, void* ptr, void* end_of_heap, struct heap_table* table);
void* heap_malloc(struct heap* heap, size_t size);
void* heap_malloc_blocks(struct heap* heap, uint32_t total_blocks);
int heap_get_start_block(struct heap* heap, uint32_t total_blocks);
void* heap_block_to_address(struct heap* heap, int start_block);
void heap_mark_blocks_taken(struct heap* heap, int start_block, int total_blocks);
void heap_free(struct heap* heap, void* ptr);
int heap_address_to_block(struct heap* heap, void* ptr);
void heap_mark_blocks_free(struct heap* heap, int starting_block);
static uint32_t heap_align_value_to_upper(uint32_t val);
static bool heap_validate_alignment(void* ptr);
static int heap_validate_table(void* ptr, void* end_of_heap, struct heap_table* table);
static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry);

#endif