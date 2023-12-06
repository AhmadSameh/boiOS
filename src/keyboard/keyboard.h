#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef int(*KEYBOARD_INIT_FUNCTION)(void);

struct process;

struct keyboard{
    KEYBOARD_INIT_FUNCTION init;
    char name[28];
    struct keyboard* next;
};

void keyboard_init(void);
void keyboard_push(char c);
char keyboard_pop(void);
void keyboard_backspace(struct process* process);

#endif