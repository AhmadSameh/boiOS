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

size_t strnlen_terminator(const char* str, size_t max, char terminator){
    size_t len = 0;
    while(str[len] != '\0' && str[len] != terminator)
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

char tolower(char c){
    if(c >= 65 && c <= 90)
        c += 32;
    return c;
}

int istrncmp(const char* str1, const char* str2, int n){
    unsigned char u1, u2;
    while(n-- > 0){
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if(u1 != u2 && tolower(u1) != tolower(u2))
            return u1 - u2;
        if(u1 == '\0')
            return 0;
    }
    return 0;
}

int strncmp(const char* str1, const char* str2, int n){
    unsigned char u1, u2;
    while(n-- > 0){
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if(u1 != u2)
            return u1 - u2;
        if(u1 == '\0')
            return 0;
    }
    return 0;
}