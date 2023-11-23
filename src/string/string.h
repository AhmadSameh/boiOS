#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdbool.h>

size_t strlen(const char* str);
size_t strnlen(const char* str, size_t max);
char* strcpy(char* dest, char* src);
bool isdigit(char c);
int tonumericdigit(char c);

#endif