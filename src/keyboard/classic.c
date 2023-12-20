#include "classic.h"
#include "keyboard.h"
#include "../kernel.h"
#include "../io/io.h"
#include "../idt/idt.h"
#include "../task/task.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void classic_keyboard_handle_interrupt(void);

static uint8_t keyboard_scan_set_one[] = {
    0x00,      ESC,      '1',  '2',  '3',      '4',     '5',  // 00 -> 06
    '6',       '7',      '8',  '9',  '0',      '-',     '=',  // 07 -> 13
    BCK_SPC,   TAB,      'Q',  'W',  'E',      'R',     'T',  // 14 -> 20
    'Y',       'U',      'I',  'O',  'P',      '[',     ']',  // 21 -> 27
    ENTER, LFT_CTR,      'A',  'S',  'D',      'F',     'G',  // 28 -> 34
    'H',       'J',      'K',  'L',  ';',     '\'',     '`',  // 35 -> 41
    LFT_SHFT, '\\',      'Z',  'X',  'C',      'V',     'B',  // 42 -> 48
    'N',       'M',      ',',  '.',  '/', RIT_SHFT,     '*',  // 49 -> 55
    LFT_ALT, SPACE, CAPS_LCK,   F1,   F2,       F3,      F4,  // 56 -> 62
    F5,         F6,       F7,   F8,   F9,      F10, NUM_LCK,  // 63 -> 69
    SCRL_LCK,  '7',      '8',  '9',  '-',      '4',     '5',  // 70 -> 76 
    '6',       '+',      '1',  '2',  '3',      '0',     '.'   // 77 -> 83
};

bool is_caps_on = false;

// static uint8_t keyboard_scan_set_two[] = {
//     0x00, 0x1B, '1', '2', '3', '4', '5',
//     '6',   '7', '8', '9', '0', '-', '=',
//     0x08, '\t', 'Q', 'W', 'E', 'R', 'T',
//     'Y',   'U', 'I', 'O', 'P', '[', ']',
//     0x0D, 0x00, 'A', 'S', 'D', 'F', 'G',
//     'H',   'J', 'K', 'L', ';', '\'', '`',
//     0x00, '\\', 'Z', 'X', 'V', 'B', 'N',
//     'M',   ',', '.', '/', 0x00, '*', 0x00,
//     0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//     '7', '8', '9', '-', '4', '5', '6',
//     '+', '1', '2', '3', '0', '.' 
// };

int classic_keyboard_init(void);

struct keyboard classic_keyboard = {
    .name = "classic",
    .init = classic_keyboard_init,
};

int classic_keyboard_init(){
    idt_retgister_interrupt_callback(ISR_KEYBOARD_INTERRUPT, classic_keyboard_handle_interrupt);
    keyboard_set_capslock(&classic_keyboard, KEYBOARD_CAPS_LOCK_OFF);    
    outb(PS2_PORT, PS2_COMMAND_ENABLE_FIRST_PORT);
    return 0;
}

uint8_t classic_keyboard_scancode_to_char(uint8_t scancode){
    size_t size_of_keyboard_set_one = sizeof(keyboard_scan_set_one) / sizeof(uint8_t);
    if(scancode > size_of_keyboard_set_one)
        return 0; 
    uint8_t c = keyboard_scan_set_one[scancode];
    if(keyboard_get_capslock(&classic_keyboard) == KEYBOARD_CAPS_LOCK_OFF){
        if(c >= 'A' && c <= 'Z')
            c += 32;
    }
    return c;
}

void classic_keyboard_handle_interrupt(){
    kernel_page();
    uint8_t scancode = insb(KEYBOARD_INPUT_PORT);
    // ignore rogue bytes
    insb(KEYBOARD_INPUT_PORT);
    if(scancode & CLASSIC_KEYBOARD_KEY_RELEASED){
        task_page();
        return;
    }
    uint8_t character = classic_keyboard_scancode_to_char(scancode);
    if(character != 0)
        keyboard_push(character);
    task_page();
}

struct keyboard* classic_init(){
    return &classic_keyboard;
}
