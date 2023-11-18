#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>
#include <stddef.h>
#include "heap.h"
#include "../../config.h"
#include "../../terminal/terminal.h"

void kheap_init(void);
void* kmalloc(size_t size);
void kfree(void* ptr);

#endif