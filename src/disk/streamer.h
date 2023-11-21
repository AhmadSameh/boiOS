#ifndef DISK_STREAMER_H
#define DISK_STREAMER_H

#include "disk.h"
#include "../memory/heap/kheap.h"
#include "../config.h"

struct disk_stream{
    int position;
    struct disk* disk;
};

struct disk_stream* diskstreamer_new(int disk_id);
int diskstreamer_seek(struct disk_stream* stream, int position);
int diskstreamer_read(struct disk_stream* stream, void* out, int total);
void diskstreamer_close(struct disk_stream* stream);

#endif