#ifndef FAT16_H
#define FAT16_H

#include "../file.h"
#include "../../status.h"
#include "../../string/string.h"

struct filesystem* fat16_init(void);

#endif