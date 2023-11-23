#include "string.h"

size_t strlen(const char* str){
    size_t len = 0;
    while(str[len])
        len++;
    return len;
}

size_t strnlen(const char* str, size_t max){
    size_t len = 0;
    while(str[len])
        len++;
    len = len > max ? max : len;
    return len;
}

bool isdigit(char c){
    return (c >= 48 && c <= 57);
}

int tonumericdigit(char c){
    return c - 48;
}

char* strcpy(char* dest, char* src){
    char* res = dest;
    while(*src){
        *dest = *src;
        src++;
        dest++;
    }
    *dest = 0x00;
    return res;
}