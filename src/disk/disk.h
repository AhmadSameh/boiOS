#ifndef DISK_H
#define DISK_H

#include "../config.h"
#include "../status.h"
#include "../io/io.h"
#include "../fs/file.h"
#include "../memory/memory.h"

// represents a real physical hard disk
#define BOIOS_DISK_TYPE_REAL    0

typedef unsigned int BOIOS_DISK_TYPE;

struct disk{
    BOIOS_DISK_TYPE type;
    int sector_size;
    int id;
    struct filesystem* filesystem;
    // private data of the filesystem
    void* fs_private;
};

void disk_search_and_init(void);
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf);

#endif