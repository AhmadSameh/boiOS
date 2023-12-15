#include "string.h"

char* sp = 0;

size_t strlen(const char* str){
    size_t len = 0;
    while(str[len])
        len++;
    return len;
}

size_t strnlen(const char* str, size_t max){
    int i = 0;
    for (i=0; i<max; i++){
        if(str[i] == 0)
            break;
    }
    return i;
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

char* strncpy(char* dest, const char* src, int count){
    int i;
    for(i=0; i<count-1; i++){
        if(src[i] == 0)
            break;
        dest[i] = src[i];
    }
    dest[i] = 0x00;
    return dest;
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

char* strtok(char* str, const char* delimeters){
    int i = 0;
    int len = strlen(delimeters);
    if(str == 0 && sp == 0)
        return 0;
    if(str && sp == 0)
        sp = str;
    char* p_start = sp;
    while(1){
        for(i=0; i<len; i++){
            if(*p_start == delimeters[i]){
                p_start++;
                break;
            }
        }
        if(i == len){
            sp = p_start;
            break;
        }
    }
    if(*sp == '\0'){
        sp = 0;
        return sp;
    }
    // find end of substring
    while(*sp != '\0'){
        for(i=0; i<len; i++){
            if(*sp == delimeters[i]){
                *sp = '\0';
                break;
            }
        }
        sp++;
        if(i < len)
            break;
    }
    return p_start;
}