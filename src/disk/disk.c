#include "disk.h"

struct disk disk;

// from from primary hard disk
int disc_read_sector(int lba, int total_blocks, void* buf){
    // send highest 8 bits of lba to hard disk controller & select master drive
    outb(0x1F6, (lba >> 24) | 0xE0);
    // send total sectors to read
    outb(0x1F2, total_blocks);
    // send more bits of lba
    outb(0x1F3, (unsigned char)(lba & 0xFF));
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    // read command
    outb(0x1f7, 0x20);
    // read 2 bytes at a time from disc controller
    unsigned short* ptr = (unsigned short*)buf;\
    for(int b=0; b<total_blocks; b++){
        // wait for buffer to be ready
        char c = insb(0x1F7); // read from bus
        // wait until c gets value 0x08 and we are ready to read
        while(!(c & 0x08))
            c = insb(0x1F7);
        // copy from hard disk to memory 2 bytes at a time from ATA controller
        for(int i=0; i<256; i++){
            *ptr = insw(0x1F0);
            ptr++;
        }
    }

    return 0;
}

void disk_search_and_init(){
    memset(&disk, 0, sizeof(disk));
    disk.type = BOIOS_DISK_TYPE_REAL;
    disk.sector_size = BOIOS_SECTOR_SIZE;
}

struct disk* disk_get(int index){
    if(index != 0)
        return 0;
    return &disk;
}

int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf){
    if(idisk != &disk)
        return -EIO;
    return disc_read_sector(lba, total, buf);
}