#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>
#include "../file.h"
#include "../../kernel.h"
#include "../../config.h"
#include "../../status.h"
#include "../../disk/disk.h"
#include "../../disk/streamer.h"
#include "../../memory/memory.h"
#include "../../memory/heap/kheap.h"
#include "../../string/string.h"

struct filesystem* fat16_init(void);

#endif