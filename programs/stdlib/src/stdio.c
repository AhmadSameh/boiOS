#include "stdio.h"
#include "boios.h"
#include "stdlib.h"
#include <stdarg.h>

int putchar(int c){
    boios_putchar((char)c);
    return 0;
}

int printf(const char* format, ...){
    va_list ap;
    const char* p;
    char* sval;
    int ival;

    va_start(ap, format);
    for(p=format; *p != 0; p++){
        if(*p != '%'){
            putchar(*p);
            continue;
        }
        switch(*++p){
            case 'i':   ival = va_arg(ap, int); print(itoa(ival));  break;
            case 's':   sval = va_arg(ap, char*);   print(sval);    break;
            default:    putchar(*p);    break;
        }
    }
    va_end(ap);
    return 0;
}

int fopen(const char* file){
    int test = (int)boios_fopen(file);
    if(test){
        printf("%d\n", test);
    }
    return test;
}

void clrscr(void){
    boios_clr_terminal();
}