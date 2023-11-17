#ifndef IO_H
#define IO_H

// input byte or word
unsigned char insb(unsigned short port);
unsigned short insw(unsigned short port);

// output byte or word
void outb(unsigned short port, unsigned char val);
void outw(unsigned short port, unsigned short val);

#endif