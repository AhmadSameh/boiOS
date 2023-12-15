#include "../stdlib/src/boios.h"
#include "../stdlib/src/stdlib.h"
#include "../stdlib/src/stdio.h"
#include "../stdlib/src/string.h"
#include <stdbool.h>


int main(int argc, char** argv){
    char words[] = "am i the mask or is the mask me";
    const char* token = strtok(words, " ");
    while(token){
        printf("%s\n", token);
        token = strtok(NULL, " ");
    }
    while(1){
    }
    return 0;
}