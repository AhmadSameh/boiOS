#ifndef ISR80H_H
#define ISR80H_H

enum system_commands{
    SYSTEM_COMMAND0_SUM,
};

void isr80h_register_commands(void);

#endif