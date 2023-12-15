#ifndef CLASSIC_KEYBOARD_H
#define CLASSIC_KEYBOARD_H

#define PS2_PORT    0x64
#define PS2_COMMAND_ENABLE_FIRST_PORT   0xAE
#define KEYBOARD_INPUT_PORT 0x60

#define ESC      0x1B
#define BCK_SPC  0x08
#define TAB      0x09
#define ENTER    0x0D
#define SPACE    0x20
#define LFT_CTR  0x1E
#define LFT_SHFT 0x1E // TODO: change value
#define RIT_SHFT 0x1E // TODO: change value
#define LFT_ALT  0x1E // TODO: change value
#define CAPS_LCK 0x1E // TODO: change value
#define F1       0x1E // TODO: change value
#define F2       0x1E // TODO: change value
#define F3       0x1E // TODO: change value
#define F4       0x1E // TODO: change value
#define F5       0x1E // TODO: change value
#define F6       0x1E // TODO: change value
#define F7       0x1E // TODO: change value
#define F8       0x1E // TODO: change value
#define F9       0x1E // TODO: change value
#define F10      0x1E // TODO: change value
#define NUM_LCK  0x1E // TODO: change value
#define SCRL_LCK 0x1E // TODO: change value


#define CLASSIC_KEYBOARD_KEY_RELEASED   0x80
#define ISR_KEYBOARD_INTERRUPT  0x21 

struct keyboard* classic_init(void);

#endif
