#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include "pparser.h"
#include "fat/fat16.h"
#include "../kernel.h"
#include "../config.h"
#include "../status.h"
#include "../terminal/terminal.h"
#include "../disk/disk.h"
#include "../memory/memory.h"
#include "../memory/heap/kheap.h"
#include "../string/string.h"

typedef unsigned int FILE_SEEK_MODE;
typedef unsigned int FILE_MODE;
typedef unsigned int FILE_STAT_FLAGS;

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

enum{
    FILE_STAT_READ_ONLY = 0b00000001,

};

struct disk;
struct path_part;

struct file_stat{
    FILE_STAT_FLAGS flags;
    uint32_t file_size;
};

// function pointer which each file system will implement this function pointer and point it to its own function (fopen, fread, etc)
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part* path, FILE_MODE mode);
typedef int (*FS_READ_FUNCTION)(struct disk* disk, void* private, uint32_t size, uint32_t nmemb, char* out);
// filesystem will need to point it to its internal resolve function, takes disk and returns if it is valid (able to process the filesystem)
typedef int (*FS_RESOLVE_FUNCTION)(struct disk* disk);
typedef int (*FS_SEEK_FUNCTION)(void* private, uint32_t offset, FILE_SEEK_MODE seek_mode);
typedef int (*FS_STAT_FUNCTION)(struct disk* disk, void* private, struct file_stat* stat);
typedef int (*FS_CLOSE_FUNCTION)(void* private);

// each filesystem will have one of this structure, which acts as an interface communicates with the filesystem
struct filesystem{
    // filesystem should return 0 from resolve if provided disk is using its filesystem
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;
    FS_READ_FUNCTION read;
    FS_SEEK_FUNCTION seek;
    FS_STAT_FUNCTION stat;
    FS_CLOSE_FUNCTION close;
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
int fopen(const char* filename, const char* mode_str);
int fread(void* ptr, uint32_t size, uint32_t nmemb, int fd);
int fseek(int fd, int offset, FILE_SEEK_MODE whence);
int fstat(int fd, struct file_stat* stat);
int fclose(int fd);
void fs_insert_filesystem(struct filesystem* filesystem);
struct filesystem* fs_resolve(struct disk* disk);

#endif