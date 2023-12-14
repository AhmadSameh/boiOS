#include "../stdlib/src/boios.h"
#include "../stdlib/src/stdlib.h"
#include "../stdlib/src/stdio.h"
#include <stdbool.h>

int main(int argc, char** argv){
    printf("What is real?\n");
    print("Hello Friend\n");
    print(itoa(69420));
    putchar('Z');
    void* ptr = malloc(512);
    free(ptr);
    char buf[1024];
    boios_terminal_readline(buf, sizeof(buf), true);
    print(buf);
    while(1){
    }
    return 0;
}