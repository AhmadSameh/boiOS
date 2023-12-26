#include "unistd.h"
#include "stdio.h"
#include "string.h"
#include "boios.h"

char cwd[128];

int chdir(const char* path){
    strncpy(cwd, path, sizeof(cwd));
    return 0;
}

char* getcwd(char* buf, size_t size){
    buf = cwd;
    return buf;
}

void sleep(int millis){
    boios_sleep(millis);
}