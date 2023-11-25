#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdbool.h>

#include "string.h"

size_t strlen(const char* str);
size_t strnlen(const char* str, size_t max);
size_t strnlen_terminator(const char* str, size_t max, char terminator);
bool isdigit(char c);
int tonumericdigit(char c);
char* strcpy(char* dest, char* src);
char tolower(char c);
int istrncmp(const char* str1, const char* str2, int n);
int strncmp(const char* str1, const char* str2, int n);

#endif