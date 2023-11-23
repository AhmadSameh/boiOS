#ifndef FILE_H
#define FILE_H

#include "pparser.h"
#include "fat/fat16.h"
#include "../config.h"
#include "../status.h"
#include "../terminal/terminal.h"
#include "../disk/disk.h"
#include "../memory/memory.h"
#include "../memory/heap/kheap.h"

typedef unsigned int FILE_SEEK_MODE;
typedef unsigned int FILE_MODE;

enum{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

enum{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

struct disk;
struct path_part;

// function pointer which each file system will implement this function pointer and point it to its own function (fopen, fread, etc)
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part* path, FILE_MODE mode);
// filesystem will need to point it to its internal resolve function, takes disk and returns if it is valid (able to process the filesystem)
typedef int (*FS_RESOLVE_FUNCTION)(struct disk* disk);

// each filesystem will have one of this structure, which acts as an interface communicates with the filesystem
struct filesystem{
    // filesystem should return 0 from resolve if provided disk is using its filesystem
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;
    // filesystems can have a name of 20 bytes e.g. FAT16, NTFS, etc
    char name[20];
};

struct file_descriptor{
    // descriptor index
    int index;
    struct filesystem* filesystem;
    // private data to the file descriptor that will be returned when file is opened and will be used to locate the file
    void* private;
    // the disk the file descriptor should be used on
    struct disk* disk;
};

void fs_init(void);
int fopen(const char* filename, const char* mode);
void fs_insert_filesystem(struct filesystem* filesystem);
struct filesystem* fs_resolve(struct disk* disk);

#endif