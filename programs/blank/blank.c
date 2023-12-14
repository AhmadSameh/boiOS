#include "boios.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char** argv){
    printf("What is real?\n");
    print("Hello Friend\n");
    print(itoa(69420));
    putchar('Z');
    void* ptr = malloc(512);
    free(ptr);
    while(1){
        if(getkey() != 0)
            print("key is pressed");
    }
    return 0;
}