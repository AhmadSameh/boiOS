#include "../io/io.h"

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
    outb(0x1f7, 0x20);
    // read 2 bytes at a time from disc controller
    unsigned short* ptr = (unsigned short*)buf;\
    for(int b=0; b<total_blocks; b++){
        // wait for buffer to be ready
        char c = insb(0x1F7); // read from bus
        // wait until c gets value 0x08
        while(!(c & 0x08))
            c = insb(0x1F7);
        // copy from hard disk to memory
        for(int i=0; i<256; i++){
            *ptr = insw(0x1F0);
            ptr++;
        }
    }

    return 0;
}