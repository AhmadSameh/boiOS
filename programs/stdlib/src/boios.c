#include "boios.h"
#include <stdbool.h>

int boios_getkeyblock(){
    int val = 0;
    do{
        val = boios_getkey();
    }while(val == 0);
    return val;
}

void boios_terminal_readline(char* out, int max, bool output_while_typing){
    int i;
    for(i=0; i<max-1; i++){
        char key = boios_getkeyblock();
        // read line when pressing enter
        if(key == 13)
            break;
        if(key == 0x1E)
            continue;
        if(output_while_typing)
            boios_putchar(key);
        // backspace
        if(key == 0x08 && i >= 1){
            out[i-1] = 0x00;
            i -=2;
            continue;
        }
        out[i] = key;
    }
    out[i] = 0x00;
}