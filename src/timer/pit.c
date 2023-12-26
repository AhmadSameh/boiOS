#include "pit.h"
#include "io/io.h"
#include "idt/idt.h"
#include "terminal/terminal.h"
#include "memory/memory.h"

int time_in_millis = 1;
int ticks;

void pit_callback(){
    ticks++;
    // if(ticks % 100 == 0)
    //     print("\n1 second passed");
    outb(0x20, 0x20);
}

void pit_init(int hz){
    if(hz <= 18)
        hz = 18;
    if(hz >= 1193181)
        hz = 1193181;
    int divisor = 1193181 / hz;
    divisor = (int)(divisor + 0.5);
    memset(&ticks, 0, sizeof(ticks));
    // uint32_t* temp = &time_in_millis;
    time_in_millis = divisor * (1000 / 1193181);
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

void sleep(int millis){ 
    ticks = 0;
    int total_ticks = millis / 10;
    int test = 0;
    do{ 
        test = ticks;
    }while(test < total_ticks);
}
