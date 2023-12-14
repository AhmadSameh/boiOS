#ifndef BOIOS_H
#define BOIOS_H

#include <stddef.h>
#include <stdbool.h>

void print(const char* msg);
int boios_getkey(void);
int boios_getkeyblock(void);
void boios_terminal_readline(char* out, int max, bool output_while_typing);
void boios_putchar(char c);
void* boios_malloc(size_t size);
void boios_free(void* ptr);


#endif