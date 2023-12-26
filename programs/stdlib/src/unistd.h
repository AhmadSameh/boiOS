#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>

int chdir(const char* path);
char* getcwd(char* buf, size_t size);
void sleep(int millis);

#endif