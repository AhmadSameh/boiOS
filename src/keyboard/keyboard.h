#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEYBOARD_CAPS_LOCK_OFF  0
#define KEYBOARD_CAPS_LOCK_ON   1

typedef int(*KEYBOARD_INIT_FUNCTION)(void);
typedef int KEYBOARD_CAPS_LOCK_STATE;

struct process;

struct keyboard{
    KEYBOARD_INIT_FUNCTION init;
    char name[28];
    struct keyboard* next;
    KEYBOARD_CAPS_LOCK_STATE capslock_state;
};

int keyboard_insert(struct keyboard* keyboard);
void keyboard_init(void);
void keyboard_set_capslock(struct keyboard* keyboard, KEYBOARD_CAPS_LOCK_STATE state);
KEYBOARD_CAPS_LOCK_STATE keyboard_get_capslock(struct keyboard* keyboard);
void keyboard_push(char c);
char keyboard_pop(void);
void keyboard_backspace(struct process* process);

#endif