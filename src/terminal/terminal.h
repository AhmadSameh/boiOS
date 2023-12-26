#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <stddef.h>
#include "../string/string.h"

#define VGA_WIDTH   80
#define VGA_HEIGHT  25

struct terminal{
    uint16_t* video_mem;
    uint16_t terminal_row;
    uint16_t terminal_col;
} __attribute__((packed));

void terminal_initialize(void);
uint16_t terminal_make_char(char c, char color);
void terminal_putchar(int x, int y, char c, char color);
void terminal_writechar(char c, char color);
void print(const char* str);

#endif