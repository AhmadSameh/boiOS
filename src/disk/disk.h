#ifndef DISK_H
#define DISK_H

int disc_read_sector(int lba, int total_blocks, void* buf);

#endif