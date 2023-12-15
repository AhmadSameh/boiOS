#ifndef BOIOS_STRING_H
#define BOIOS_STRING_H

#include <stdbool.h>
#include <stddef.h>

size_t strlen(const char* str);
size_t strnlen(const char* str, size_t max);
size_t strnlen_terminator(const char* str, size_t max, char terminator);
bool isdigit(char c);
int tonumericdigit(char c);
char* strcpy(char* dest, char* src);
char* strncpy(char* dest, const char* src, int count);
char tolower(char c);
int istrncmp(const char* str1, const char* str2, int n);
int strncmp(const char* str1, const char* str2, int n);
char* strtok(char* str, const char* delimeters);

#endif