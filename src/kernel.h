#ifndef KERNEL_H
#define KERNEL_H    

#include <stdint.h>
#include <stddef.h>
#include "terminal/terminal.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "fs/pparser.h"

#define BOIOS_MAX_PATH  128

#endif
