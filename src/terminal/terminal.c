#include "terminal.h"

struct terminal terminal;

void terminal_initialize(){
    terminal.video_mem = (uint16_t*)(0xB8000);
    terminal.terminal_row = 0;
    terminal.terminal_col = 0;
    for(int y=0; y<VGA_HEIGHT; y++){
        for(int x=0; x<VGA_WIDTH; x++)
            terminal_putchar(x, y, ' ', 0);
    }
}  

void terminal_putchar(int x, int y, char c, char color){
    terminal.video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, color);
}

uint16_t terminal_make_char(char c, char color){
    return (color << 8) | c;
}

void terminal_backspace(){
    if(terminal.terminal_col == 0 && terminal.terminal_row == 0)
        return;
    if(terminal.terminal_col == 0){
        terminal.terminal_row--;
        terminal.terminal_col = VGA_WIDTH;
    }
    terminal.terminal_col--;
    terminal_writechar(' ', 15);
    if(terminal.terminal_col == 0){
        terminal.terminal_row--;
        terminal.terminal_col = VGA_WIDTH;
    }
    terminal.terminal_col--;
}

void terminal_writechar(char c, char color){
    if(c == '\n'){
        terminal.terminal_row += 1;
        terminal.terminal_col = 0;
        return;
    }
    if(c == 0x08){
        terminal_backspace();
        return;
    }
    terminal_putchar(terminal.terminal_col, terminal.terminal_row, c, color);
    terminal.terminal_col += 1;
    if(terminal.terminal_col >= VGA_WIDTH){
        terminal.terminal_col = 0;
        terminal.terminal_row += 1;
    }
}

void print(const char* str){
    size_t len = strlen(str);
    for(int i=0; i<len; i++)
        terminal_writechar(str[i], 15);
}