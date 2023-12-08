#ifndef CLASSIC_KEYBOARD_H
#define CLASSIC_KEYBOARD_H

#define PS2_PORT    0x64
#define PS2_COMMAND_ENABLE_FIRST_PORT   0xAE
#define KEYBOARD_INPUT_PORT 0x60

#define CLASSIC_KEYBOARD_KEY_RELEASED   0X80
#define ISR_KEYBOARD_INTERRUPT  0x21 

struct keyboard* classic_init(void);

#endif
