#ifndef BOIOS_H
#define BOIOS_H

#include <stddef.h>

void print(const char* msg);
int getkey(void);
void boios_putchar(char c);
void* boios_malloc(size_t size);
void boios_free(void* ptr);


#endif