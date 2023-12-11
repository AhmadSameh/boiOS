#include <stdbool.h>
#include "streamer.h"

struct disk_stream* diskstreamer_new(int disk_id){
    struct disk* disk = disk_get(disk_id);
    if(!disk)
        return 0;
    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->position = 0;
    streamer->disk = disk;
    return streamer;
}

int diskstreamer_seek(struct disk_stream* stream, int position){
    stream->position = position;
    return 0;
}

// read from streamer total amount of bytes into the out pointer
int diskstreamer_read(struct disk_stream* stream, void* out, int total){
    int sector = stream->position / BOIOS_SECTOR_SIZE;
    int offset = stream->position % BOIOS_SECTOR_SIZE;
    int total_to_read = total;
    bool overflow = (offset + total_to_read) >= BOIOS_SECTOR_SIZE;
    char buf[BOIOS_SECTOR_SIZE];
    if(overflow)
        total_to_read -= (offset + total_to_read) - BOIOS_SECTOR_SIZE;

    int response = disk_read_block(stream->disk, sector, 1, buf);
    if(response < 0)
        goto out;
    // read from buffer where sector is loaded into out
    for(int i=0; i<total_to_read; i++)
        *(char*)out++ = buf[offset + i];
    // adjust stream
    stream->position += total_to_read;
    // recursion is used to read past the first sector, out points to end of sector at all times
    if(overflow)
        response = diskstreamer_read(stream, out, total-total_to_read);
out:
    return response;
}

void diskstreamer_close(struct disk_stream* stream){
    kfree(stream);
}