#ifndef PIT_H
#define PIT_H

#define PIT_CHANNEL0_PORT   0x40
#define PIT_CHANNEL1_PORT   0x41
#define PIT_CHANNEL2_PORT   0x42
#define PIT_COMMAND_REG     0x43   

#define PIT_CHANNEL0    0b00000000
#define PIT_CHANNEL1    0b10000000
#define PIT_CHANNEL2    0b01000000
#define PIT_READ_BACK   0b11000000

void pit_init(int hz);
void pit_callback(void);
void sleep(int millis);

#endif