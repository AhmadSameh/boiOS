#ifndef KERNEL_H
#define KERNEL_H

#define VGA_WIDTH   80
#define VGA_HEIGHT  20

void kernel_main(void);
void terminal_initialize(void);
uint16_t terminal_make_char(char c, char color);
void terminal_putchar(int x, int y, char c, char color);
void terminal_writechar(char c, char color);

#endif