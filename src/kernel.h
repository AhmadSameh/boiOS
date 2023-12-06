#ifndef KERNEL_H
#define KERNEL_H    

#include <stdint.h>
#include <stddef.h>
#include "terminal/terminal.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "disk/streamer.h"
#include "fs/pparser.h"
#include "fs/file.h"
#include "gdt/gdt.h"
#include "task/tss.h"
#include "config.h"

#define BOIOS_MAX_PATH  128

#define ERROR(value)    (void*)(value)
#define ERROR_I(value)  (int)(value)
#define ISERR(value)    ((int)value < 0)

void panic(const char* msg);
void kernel_page(void);
void kernel_registers(void);

#endif
